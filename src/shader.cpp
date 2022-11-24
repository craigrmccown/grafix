#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"

const int info_log_buf_size = 512;

GlShaderCompiler::GlShaderCompiler(GLenum type)
{
    id = glCreateShader(type);
}

GlShaderCompiler::~GlShaderCompiler()
{
    // Automatically clean up shader. If shader is attached to a program, it
    // will only be marked for deletion, but resources will remain until
    // detached.
    glDeleteShader(id);
}

void GlShaderCompiler::compile(const char *src)
{
    // Pass NULL as final arg because we assume shader source is null-terminated
    glShaderSource(id, 1, &src, NULL); 
    glCompileShader(id);

    char logBuf[info_log_buf_size];
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, info_log_buf_size, NULL, logBuf);

        // Info log should be null-terminated
        throw std::runtime_error("Failed to compile shader\n" + std::string(logBuf));
    }
}

unsigned int GlShaderCompiler::getId()
{
    return id;
}


GlProgramLinker::GlProgramLinker()
{
    id = glCreateProgram();
}

GlProgramLinker::~GlProgramLinker()
{
    // Deletes program and detaches all shaders, thereby deleting any
    // shaders marked for deletion
    glDeleteProgram(id);
}

void GlProgramLinker::link(std::vector<unsigned int> shaders)
{
    for (unsigned int shader : shaders) {
        glAttachShader(id, shader);
    }
    glLinkProgram(id);

    // Once the program is linked, we no longer need the shaders. Detach them, but
    // because they were not locally created, don't delete them.
    for (unsigned int shader : shaders) {
        glDetachShader(id, shader);
    }

    char logBuf[info_log_buf_size];
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, info_log_buf_size, NULL, logBuf);

        // Info log should be null-terminated
        throw std::runtime_error("Failed to link shader program\n" + std::string(logBuf));
    }
}

unsigned int GlProgramLinker::getId()
{
    return id;
}

void Shader::build(std::vector<ShaderSrc> srcs)
{
    // only build shader program once
    if (isBuilt())
    {
        return;
    }

    // Map shader source code to compiled OpenGL objects. Once the program is linked,
    // these objects will fall out of scope and be deallocated.
    std::vector<std::unique_ptr<GlShaderCompiler>> compilers;
    for (ShaderSrc src : srcs) {
        std::unique_ptr<GlShaderCompiler> compiler(new GlShaderCompiler(src.type));

        // TODO: Report all compilation errors instead of first
        compiler->compile(src.code.c_str());

        // Only add shaders once they are successfully compiled so that invalid shader
        // objects can be cleaned up opportunistically.
        compilers.push_back(std::move(compiler));
    }

    // Extract shader IDs, using a reference to avoid copies
    std::vector<unsigned int> shaders;
    for (std::unique_ptr<GlShaderCompiler> const &comp : compilers) {
        shaders.push_back(comp->getId());
    }

    program = std::make_unique<GlProgramLinker>();
    program->link(shaders);
}

void Shader::use()
{
    if (!isBuilt())
    {
        throw std::logic_error("Must call build before calling use");
    }
    glUseProgram(program->getId());
}

bool Shader::isBuilt()
{
    return !!program;
}

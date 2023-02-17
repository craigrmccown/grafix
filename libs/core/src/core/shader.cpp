#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"

const int info_log_buf_size = 512;

std::string buildIndexedUniformName(std::string name, std::string property, int idx)
{
    // Example: myname[0].myproperty
    return name + "[" + std::to_string(idx) + "]." + property;
}

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

void GlShaderCompiler::compile(std::string src)
{
    const char *srcPtr = src.c_str();

    // Pass NULL as final arg because we assume shader source is null-terminated
    glShaderSource(id, 1, &srcPtr, NULL);
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

Shader::Shader() {
    program = std::make_unique<GlProgramLinker>();
}

void Shader::build(std::vector<ShaderSrc> srcs)
{
    // Map shader source code to compiled OpenGL objects. Once the program is linked,
    // these objects will fall out of scope and be deallocated.
    std::vector<std::unique_ptr<GlShaderCompiler>> compilers;
    for (ShaderSrc src : srcs) {
        auto compiler = std::make_unique<GlShaderCompiler>(src.type);

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

    program->link(shaders);
}

void Shader::use()
{
    glUseProgram(program->getId());
}

void Shader::setUniformMat4(std::string name, glm::mat4 mat) {
    unsigned int uniformId = glGetUniformLocation(program->getId(), name.c_str());
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniformVec3(std::string name, glm::vec3 vec) {
    unsigned int uniformId = glGetUniformLocation(program->getId(), name.c_str());
    glUniform3f(uniformId, vec.x, vec.y, vec.z);
}

void Shader::setUniformVec3Element(std::string name, std::string property, int idx, glm::vec3 vec) {
    setUniformVec3(buildIndexedUniformName(name, property, idx), vec);
}

void Shader::setUniformFloat(std::string name, float f) {
    unsigned int uniformId = glGetUniformLocation(program->getId(), name.c_str());
    glUniform1f(uniformId, f);
}

void Shader::setUniformFloatElement(std::string name, std::string property, int idx, float f) {
    setUniformFloat(buildIndexedUniformName(name, property, idx), f);
}

GLenum mapPathToShaderType(std::string path)
{
    std::filesystem::path fsPath = path;
    std::string ext = fsPath.extension();

    if (ext == ".vert")
    {
        return GL_VERTEX_SHADER;
    }
    else if (ext == ".frag")
    {
        return GL_FRAGMENT_SHADER;
    }
    else
    {
        throw std::logic_error("Unsupported file type: " + path);
    }
}

ShaderSrc loadShaderSrc(std::string path)
{
    GLenum type = mapPathToShaderType(path);

    // Open file for reading
    std::ifstream file(path);
    if (file.fail()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    // Read the entire file into memory
    std::stringstream buf;
    buf << file.rdbuf();

    return ShaderSrc(type, buf.str());
}

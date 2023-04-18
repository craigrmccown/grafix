#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"

const int info_log_buf_size = 512;

static std::string buildIndexedUniformName(std::string name, std::string property, int idx)
{
    // Example: myname[0].myproperty
    return name + "[" + std::to_string(idx) + "]." + property;
}

namespace orc
{
    OpenGLShaderSource::OpenGLShaderSource(GLenum type, const std::string &src)
    {
        id = glCreateShader(type);
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
            glDeleteShader(id);

            // Info log should be null-terminated
            throw std::runtime_error("Failed to compile shader\n" + std::string(logBuf));
        }
    }

    OpenGLShaderSource::~OpenGLShaderSource()
    {
        // Automatically clean up shader. If shader is attached to a program, it
        // will only be marked for deletion, but resources will remain until
        // detached.
        glDeleteShader(id);
    }

    unsigned int OpenGLShaderSource::GetId()
    {
        return id;
    }

    OpenGLShaderProgram::OpenGLShaderProgram(std::vector<unsigned int> shaders)
    {
        id = glCreateProgram();
        for (unsigned int shader : shaders)
        {
            glAttachShader(id, shader);
        }
        glLinkProgram(id);

        // Once the program is linked, we no longer need the shaders. Detach them, but
        // because they were not locally created, don't delete them.
        for (unsigned int shader : shaders)
        {
            glDetachShader(id, shader);
        }

        char logBuf[info_log_buf_size];
        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, info_log_buf_size, NULL, logBuf);
            glDeleteProgram(id);

            // Info log should be null-terminated
            throw std::runtime_error("Failed to link shader program\n" + std::string(logBuf));
        }
    }

    OpenGLShaderProgram::~OpenGLShaderProgram()
    {
        // Deletes program and detaches all shaders, thereby deleting any
        // shaders marked for deletion
        glDeleteProgram(id);
    }

    unsigned int OpenGLShaderProgram::GetId()
    {
        return id;
    }

    OpenGLShader::OpenGLShader(const std::string &vSrc, const std::string &fSrc)
    {
        // Compile each shader. Once the program is linked, these objects will
        // fall out of scope and be deallocated.
        OpenGLShaderSource vShader(GL_VERTEX_SHADER, vSrc), fShader(GL_FRAGMENT_SHADER, fSrc);
        program = std::make_unique<OpenGLShaderProgram>(std::vector<unsigned int>{ vShader.GetId(), fShader.GetId() });
    }

    void OpenGLShader::Use()
    {
        glUseProgram(program->GetId());
    }

    void OpenGLShader::SetUniformMat4(std::string name, glm::mat4 mat) {
        unsigned int uniformId = glGetUniformLocation(program->GetId(), name.c_str());
        glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void OpenGLShader::SetUniformVec3(std::string name, glm::vec3 vec) {
        unsigned int uniformId = glGetUniformLocation(program->GetId(), name.c_str());
        glUniform3f(uniformId, vec.x, vec.y, vec.z);
    }

    void OpenGLShader::SetUniformVec3Element(std::string name, std::string property, int idx, glm::vec3 vec) {
        SetUniformVec3(buildIndexedUniformName(name, property, idx), vec);
    }

    void OpenGLShader::SetUniformFloat(std::string name, float f) {
        unsigned int uniformId = glGetUniformLocation(program->GetId(), name.c_str());
        glUniform1f(uniformId, f);
    }

    void OpenGLShader::SetUniformFloatElement(std::string name, std::string property, int idx, float f) {
        SetUniformFloat(buildIndexedUniformName(name, property, idx), f);
    }

    static std::string loadShaderSrc(const std::string &path)
    {
        // Open file for reading
        std::ifstream file(path);
        if (file.fail()) {
            throw std::runtime_error("Could not open file: " + path);
        }

        // Read the entire file into memory
        std::stringstream buf;
        buf << file.rdbuf();

        return buf.str();
    }

    std::unique_ptr<OpenGLShader> LoadShaderFromFiles(const std::string &vPath, const std::string &fPath)
    {
        return std::make_unique<OpenGLShader>(loadShaderSrc(vPath), loadShaderSrc(fPath));
    }
}

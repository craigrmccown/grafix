#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace orc
{
    /**
     * Compiles shader source and manages OpenGL objects
     */
    class OpenGLShaderSource 
    {
        public:
        OpenGLShaderSource(GLenum type, std::string src);

        ~OpenGLShaderSource();

        // Copy constructor and copy assignment are disabled because this class
        // manages OpenGL resources
        OpenGLShaderSource(const OpenGLShaderSource &other) = delete;
        void operator=(const OpenGLShaderSource &other) = delete;

        unsigned int GetId();

        private:
        unsigned int id;
    };

    /**
     * Links a shader program from pre-compiled shader objects
     */
    class OpenGLShaderProgram 
    {
        public:
        OpenGLShaderProgram(std::vector<unsigned int> shaders);

        ~OpenGLShaderProgram();

        // Copy constructor and copy assignment are disabled because this class
        // manages OpenGL resources
        OpenGLShaderProgram(const OpenGLShaderProgram &other) = delete;
        void operator=(const OpenGLShaderProgram &other) = delete;

        unsigned int GetId();

        private:
        unsigned int id;
    };

    struct ShaderSrc
    {
        GLenum type;
        std::string code;

        ShaderSrc(GLenum type, std::string code) : type(type), code(code) { }
    };

    /**
     * Primary API by which to create and use OpenGL shaders
     */
    class OpenGLShader
    {
        public:
        OpenGLShader(std::vector<ShaderSrc> srcs);

        // Associate this shader with the current OpenGL context. Subsequent
        // draw commands will be rendered using this shader.
        void Use();

        void SetUniformMat4(std::string name, glm::mat4 mat);
        void SetUniformVec3(std::string name, glm::vec3 vec);
        void SetUniformFloat(std::string name, float f);
        void SetUniformVec3Element(std::string name, std::string property, int idx, glm::vec3 vec);
        void SetUniformFloatElement(std::string name, std::string property, int idx, float f);

        private:
        std::unique_ptr<OpenGLShaderProgram> program;
    };

    /**
     * Compiles and links shader program from GLSL source files. Shaders are
     * classified based on the extension of each file.
     *
     * Supported extensions are .vert and .frag
     */
    std::unique_ptr<OpenGLShader> LoadShaderFromFiles(std::vector<std::string> paths);
}

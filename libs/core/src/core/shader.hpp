#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

/**
 * Compiles shader source and manages OpenGL objects
 */
class GlShaderCompiler
{
    public:
    GlShaderCompiler(GLenum type);

    ~GlShaderCompiler();

    void compile(std::string src);

    unsigned int getId();

    private:
    unsigned int id;
};

/**
 * Creates a shader program from pre-compiled shader objects
 */
class GlProgramLinker
{
    public:
    GlProgramLinker();

    ~GlProgramLinker();

    void link(std::vector<unsigned int> shaders);

    unsigned int getId();

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
 * 
 * Must call build before calling use. Because of underlying OpenGL objects, copies should
 * be avoided.
 */
class Shader
{
    public:
    Shader();

    void build(std::vector<ShaderSrc> srcs);

    void use();

    void setUniformMat4(std::string name, glm::mat4 mat);
    void setUniformVec3(std::string name, glm::vec3 vec);
    void setUniformFloat(std::string name, float f);
    void setUniformVec3Element(std::string name, std::string property, int idx, glm::vec3 vec);
    void setUniformFloatElement(std::string name, std::string property, int idx, float f);

    private:
    std::unique_ptr<GlProgramLinker> program;
};

/**
 * Loads shader source code and classifies its type based on extension
 *
 * Supported extensions are .vert and .frag
 */
ShaderSrc loadShaderSrc(std::string path);

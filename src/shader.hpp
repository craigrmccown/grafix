#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glad/glad.h>

/**
 * Compiles shader source and manages OpenGL objects
 */
class GlShaderCompiler
{
    public:
    GlShaderCompiler(GLenum type);

    ~GlShaderCompiler();

    void compile(const char *src);

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
    void build(std::vector<ShaderSrc> srcs);

    void use();

    private:
    std::unique_ptr<GlProgramLinker> program;

    bool isBuilt();
};

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders.hpp"

// TODO: Find a better way to embed shader source
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

unsigned int compileShader(GLenum type, const char *source) {
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, NULL); // pass NULL as final arg because we assume shader source is null-terminated
    glCompileShader(shaderId);
    return shaderId;
}

unsigned int compileAndLinkShaders() {
    unsigned int vShaderId, fShaderId, programId;
    vShaderId = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    fShaderId = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    programId = glCreateProgram();
    
    glAttachShader(programId, vShaderId);
    glAttachShader(programId, fShaderId);
    glLinkProgram(programId);

    // We don't need these after linking
    glDeleteShader(vShaderId);
    glDeleteShader(fShaderId);

    return programId;
}

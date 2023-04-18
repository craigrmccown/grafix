#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <glad/glad.h>
#include <orc/shader.hpp>

const std::string vertexShader =
"#version 330 core\n"
"layout (location = 0) in vec3 va_xyz;\n"
"layout (location = 1) in vec2 va_uv;\n"
"out vec4 vs_out_color;\n"
"uniform sampler2D u_baseColor;\n"
"uniform mat4 u_transformMx;\n"
"void main() {\n"
"  gl_Position = u_transformMx * vec4(va_xyz, 1.0);\n"
"  vs_out_color = texture(u_baseColor, va_uv);\n"
"}\n";

const std::string fragmentShader =
"#version 330 core\n"
"in vec4 vs_out_color;"
"out vec4 fs_out_color;\n"
"uniform vec3 u_color;\n"
"void main() {\n"
"  fs_out_color = vec4(u_color, 1.0) * vs_out_color;\n"
"}\n";

TEST_CASE("Compile shaders", "[orc]") {
    std::cout << vertexShader << std::endl;
    CHECK_NOTHROW(orc::OpenGLShader(vertexShader, fragmentShader));
    CHECK_THROWS(orc::OpenGLShader(vertexShader, "#version invalid!!"));
}

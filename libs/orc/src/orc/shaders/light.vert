#version 330 core

layout (location = 0) in vec3 _coord;

uniform mat4 transformMx;

void main()
{
    gl_Position = transformMx * vec4(_coord, 1.0);
}

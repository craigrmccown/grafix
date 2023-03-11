#version 330 core

layout (location = 0) in vec3 _coord;

uniform mat4 transformMat;

void main()
{
    gl_Position = transformMat * vec4(_coord, 1.0);
}

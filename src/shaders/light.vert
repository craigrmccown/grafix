#version 330 core

layout (location = 0) in vec3 _coord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(_coord, 1.0);
}

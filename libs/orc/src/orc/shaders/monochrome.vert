#version 330 core

layout (location = 0) in vec3 va_coords;

uniform mat4 u_transformMx;

void main()
{
    gl_Position = u_transformMx * vec4(va_coords, 1.0);
}

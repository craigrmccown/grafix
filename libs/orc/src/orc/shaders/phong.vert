#version 330 core

layout (location = 0) in vec3 va_coords;
layout (location = 1) in vec3 va_normal;
layout (location = 2) in vec2 va_texCoords;

out vec2 vs_out_texCoords;
out vec3 vs_out_normal;
out vec3 vs_out_fragPos;

uniform mat4 u_transformMx;
uniform mat4 u_modelMx;

void main()
{
    gl_Position = u_transformMx * vec4(va_coords, 1.0);
    vs_out_texCoords = va_texCoords;

    // Compute fragment position and normal direction in world space by applying
    // model transformation
    vs_out_normal = vec3(u_modelMx * vec4(va_normal, 0.0));
    vs_out_fragPos = vec3(u_modelMx * vec4(va_coords, 1.0));
}

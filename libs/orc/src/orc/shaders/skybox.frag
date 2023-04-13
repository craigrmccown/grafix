#version 330 core

in vec3 coord;

uniform samplerCube cubemap;

out vec4 color;

void main()
{
    color = vec4(vec3(texture(cubemap, coord)), 1.0);
}

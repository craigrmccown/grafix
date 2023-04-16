#version 330 core

in vec3 vs_out_coords;

uniform samplerCube u_cubemap;

out vec4 fs_out_color;

void main()
{
    fs_out_color = vec4(vec3(texture(u_cubemap, vs_out_coords)), 1.0);
}

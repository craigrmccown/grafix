#version 330 core

out vec4 fs_out_color;

uniform vec3 u_color;

void main()
{
    fs_out_color = vec4(u_color, 1.0);
}

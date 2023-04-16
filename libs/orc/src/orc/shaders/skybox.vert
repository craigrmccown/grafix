#version 330 core

layout (location = 0) in vec3 va_coords;

uniform mat4 u_transformMx;

out vec3 vs_out_coords;

void main()
{
    // We use the original (untransformed, model space) coordinate to sample the
    // cubemap in the fragment shader
    vs_out_coords = va_coords;

    // The z-component of the fragment is always set to the furthest value in
    // NDC, 1, by setting it equal to w (perspective division will divide every
    // component by w)
    vec4 pos = u_transformMx * vec4(va_coords, 1.0);
    gl_Position = pos.xyww;
}

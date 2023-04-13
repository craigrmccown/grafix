#version 330 core

layout (location = 0) in vec3 _coord;

uniform mat4 transformMx;

out vec3 coord;

void main()
{
    // We use the original (untransformed, model space) coordinate to sample the
    // cubemap in the fragment shader
    coord = _coord;

    // The z-component of the fragment is always set to the furthest value in
    // NDC, 1, by setting it equal to w (perspective division will divide every
    // component by w)
    vec4 pos = transformMx * vec4(_coord, 1.0);
    gl_Position = pos.xyww;
}

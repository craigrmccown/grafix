#version 330 core

layout (location = 0) in vec3 _coord;
layout (location = 1) in vec2 _texCoord;
layout (location = 2) in vec3 _normal;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 transformMat;
uniform mat4 modelMat;

void main()
{
    gl_Position = transformMat * vec4(_coord, 1.0);
    texCoord = _texCoord;
    normal = _normal;

    // Compute fragment position in world-space by applying model transformation
    fragPos = vec3(modelMat * vec4(_coord, 1.0));
}

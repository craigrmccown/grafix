#version 330 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D tex;
uniform vec3 lightColor;

void main()
{
    float ambientCoefficient = 0.2;
    color = texture(tex, texCoord);
    color = color * ambientCoefficient;
    color = vec4(color.xyz * lightColor, 1.0);
}

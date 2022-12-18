#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

uniform sampler2D tex;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // Use simple constant for ambient lighting
    float ambient = 0.2;

    // Calculate diffuse by taking the dot product between unit vectors to
    // obtain cos(theta) where theta is the angle between the normal vector and
    // the light direction. The more direct the light, the brighter the color.
    vec3 lightDir = normalize(lightPos - fragPos);
    float cosTheta = dot(normalize(normal), lightDir);
    float diffuse = max(cosTheta, 0.0);

    // Sample texture and apply lighting to get final color values
    vec3 texColor = vec3(texture(tex, texCoord));
    color = vec4(texColor * (ambient + diffuse) * lightColor, 1.0);
}

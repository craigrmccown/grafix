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

    // Calculate specular by taking the dot product between the view direction
    // and the direction of the light's reflection. Because we are working in
    // view space already, the origin is always (0,0,0). The shininess controls
    // the spread of the glare, and the brightness controls the intensity.
    float shininess = 64;
    float brightness = 0.8;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Multiply by diffuse lighting so that we only get specular highlights on
    // surfaces that aren't supposed to reflect light. This enforces that we
    // won't show specular highlights unless there is already some diffuse
    // light reflecting from a surface.
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * brightness * diffuse;

    // Sample texture and apply lighting to get final color values
    vec3 texColor = vec3(texture(tex, texCoord));
    color = vec4(texColor * (ambient + diffuse + specular) * lightColor, 1.0);
}

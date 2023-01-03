#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

struct PointLight {
    vec3 color;
    vec3 position;

    float ambient;
    float diffuse;
    float specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    float shininess;
};

#define NUM_POINT_LIGHTS 4

uniform sampler2D tex;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform Material material;

vec3 computePointLighting(PointLight light, vec3 fragPos, vec3 normal)
{
    // Attenuation reduces the intensity of lighting effects as an object gets
    // farther from the light source
    vec3 lightVec = light.position - fragPos;
    vec3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * lightDistance * lightDistance);

    // Use simple constant for ambient lighting
    float ambient = light.ambient;

    // Calculate diffuse by taking the dot product between unit vectors to
    // obtain cos(theta) where theta is the angle between the normal vector and
    // the light direction. The more direct the light, the brighter the color.
    float reflectionAngle = max(dot(normalize(normal), lightDir), 0.0);
    float diffuse = light.diffuse * reflectionAngle;

    // Calculate specular by using the angle between the camera and the
    // direction of the light's reflection. Because we are working in view space
    // already, the origin is always (0,0,0). The shininess controls the spread
    // of the glare, and the brightness controls the intensity of the reflected
    // light.
    //
    // To efficently compute the angle between the camera and the light's
    // reflection, we can create a bisector by averaging the direction from the
    // fragment to the light with the direction from the fragment to the camera.
    // Then, this vector can be compared to the surface normal using a dot
    // product. The closer the direction of the bisector to the direction of the
    // surface normal, the closer the camera is to the light's reflection.
    vec3 viewDir = normalize(-fragPos);
    vec3 bisector = normalize((lightDir + viewDir) / 2);

    // Multiply by reflection angle so that we only get specular highlights on
    // surfaces that are supposed to reflect light.
    float specular = pow(max(dot(bisector, normal), 0.0), material.shininess) * light.specular * reflectionAngle;

    // Compute final lighting values based on light properties
    return attenuation * (ambient + diffuse + specular) * light.color;
}

void main()
{
    vec3 lighting = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        lighting += computePointLighting(pointLights[i], fragPos, normal);
    }

    // Sample texture and apply lighting to get final color values
    vec3 texColor = vec3(texture(tex, texCoord));
    color = vec4(lighting * texColor, 1.0);
}

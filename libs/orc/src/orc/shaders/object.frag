#version 330 core

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

struct Phong {
    float ambient;
    float diffuse;
    float specular;
};

struct GlobalLight {
    vec3 color;
    vec3 direction;

    Phong phong;
};

struct OmniLight {
    vec3 color;
    vec3 position;

    Phong phong;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 color;
    vec3 direction;
    vec3 position;

    // cosine of angle between the light direction and a fragment where lesser
    // angles receive full brightness
    float inner;

    // cosine of angle between the light direction and a fragment where lesser
    // angles receive partial brightness, and greater angles receive none
    float outer;

    Phong phong;
};

#define NUM_OMNI_LIGHTS 4

uniform sampler2D tex;
uniform vec3 cameraPosition;
uniform GlobalLight globalLight;
uniform OmniLight omniLights[NUM_OMNI_LIGHTS];
uniform SpotLight spotLight;

float computeLighting(Phong phong, vec3 lightDir, vec3 fragPos, vec3 normal)
{
    // Use simple constant for ambient lighting
    float ambient = phong.ambient;

    // Calculate diffuse by taking the dot product between unit vectors to
    // obtain cos(theta) where theta is the angle between the normal vector and
    // the light direction. The more direct the light, the brighter the color.
    float reflectionAngle = max(dot(normalize(normal), lightDir), 0.0);
    float diffuse = phong.diffuse * reflectionAngle;

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
    vec3 viewDir = normalize(cameraPosition-fragPos);
    vec3 bisector = normalize((lightDir + viewDir) / 2);

    // Multiply by reflection angle so that we only get specular highlights on
    // surfaces that are supposed to reflect light.
    float specular = pow(max(dot(bisector, normal), 0.0), 64.0) * phong.specular * reflectionAngle;

    return ambient + diffuse + specular;
}

vec3 computeGlobalLighting(GlobalLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    return computeLighting(light.phong, lightDir, fragPos, normal) * light.color;
}

vec3 computePointLighting(OmniLight light, vec3 fragPos, vec3 normal)
{
    // Attenuation reduces the intensity of lighting effects as an object gets
    // farther from the light source
    vec3 lightVec = light.position - fragPos;
    vec3 lightDir = normalize(lightVec);
    float lightDistance = length(lightVec);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * lightDistance * lightDistance);

    return attenuation * computeLighting(light.phong, lightDir, fragPos, normal) * light.color;
}

vec3 computeSpotLighting(SpotLight light, vec3 fragPos, vec3 normal)
{
    // Take the angle between the light direction and the vector from the
    // fragment to the light. If that angle is less than inner, full brightness
    // is applied. Brightness fades as the angle approaches outer. All angles
    // are expected to be represented as cosines, not degrees or radians.
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(light.position - fragPos);
    float cosTheta = dot(lightDir, viewDir);
    float brightness = clamp((cosTheta - light.outer) / (light.inner - light.outer), 0.1, 1.0);

    return brightness * computeLighting(light.phong, lightDir, fragPos, normal) * light.color;
}

void main()
{
    vec3 lighting = computeGlobalLighting(globalLight, fragPos, normal);

    for (int i = 0; i < NUM_OMNI_LIGHTS; i++)
    {
        lighting += computePointLighting(omniLights[i], fragPos, normal);
    }

    lighting += computeSpotLighting(spotLight, fragPos, normal);

    // Sample texture and apply lighting to get final color values
    vec3 texColor = vec3(texture(tex, texCoord));
    color = vec4(lighting * texColor, 1.0);
}

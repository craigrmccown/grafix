#version 330 core

#define NUM_OMNI_LIGHTS 4

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

    float brightness;
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

in vec2 vs_out_texCoords;
in vec3 vs_out_normal;
in vec3 vs_out_fragPos;

out vec4 fs_out_color;

uniform sampler2D u_texture;
uniform vec3 u_cameraPosition;
uniform GlobalLight u_globalLight;
uniform OmniLight u_omniLights[NUM_OMNI_LIGHTS];
uniform SpotLight u_spotLight;

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
    vec3 viewDir = normalize(u_cameraPosition-fragPos);
    vec3 bisector = normalize((lightDir + viewDir) / 2);

    // Multiply by reflection angle so that we only get specular highlights on
    // surfaces that are supposed to reflect light.
    float specular = pow(max(dot(bisector, normal), 0.0), 128.0) * phong.specular * reflectionAngle;

    return ambient + diffuse + specular;
}

vec3 computeGlobalLighting(GlobalLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    return computeLighting(light.phong, lightDir, fragPos, normal) * light.color;
}

vec3 computePointLighting(OmniLight light, vec3 fragPos, vec3 normal)
{
    vec3 lightVec = light.position - fragPos;
    vec3 lightDir = normalize(lightVec);

    // Attenuation reduces the intensity of lighting effects as an object gets
    // farther from the light source. We use a linear attenuation function
    // instead of quadratic because we assume gamma correction is enabled, which
    // will transform color values to an exponential scale.
    float attenuation = 1.0 / length(lightVec);

    return light.brightness * attenuation * computeLighting(light.phong, lightDir, fragPos, normal) * light.color;
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
    vec3 lighting = computeGlobalLighting(u_globalLight, vs_out_fragPos, vs_out_normal);

    for (int i = 0; i < NUM_OMNI_LIGHTS; i++)
    {
        lighting += computePointLighting(u_omniLights[i], vs_out_fragPos, vs_out_normal);
    }

    lighting += computeSpotLighting(u_spotLight, vs_out_fragPos, vs_out_normal);

    // Sample texture and apply lighting to get final color values
    vec4 texColor = texture(u_texture, vs_out_texCoords);
    fs_out_color = vec4(lighting * texColor.rgb, texColor.a);
}

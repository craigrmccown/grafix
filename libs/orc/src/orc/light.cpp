#include "light.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    Light::Light()
        : color(glm::vec3(1.0f))
        , phong(Phong{.Ambient = 0.15f, .Diffuse = 0.75f, .Specular = 0.6f})
        {}

    void Light::SetColor(float r, float g, float b)
    {
        color = glm::vec3(r, g, b);
    }

    void Light::SetPhong(float ambient, float diffuse, float specular)
    {
        phong = Phong{.Ambient = ambient, .Diffuse = diffuse, .Specular = specular};
    }

    glm::vec3 Light::GetColor() const
    {
        return color;
    }

    Phong Light::GetPhong() const
    {
        return phong;
    }

    OmniLight::OmniLight()
    {
        SetRadius(50.0f);
    }

    void OmniLight::Dispatch(ObjVisitor &visitor)
    {
        visitor.VisitOmniLight(this);
    }

    void OmniLight::SetRadius(float radius)
    {
        // TODO: Tweak attenuation function for more realistic lighting effect
        attenuation = Attenuation{
            .Constant = 1.0f,
            .Linear = 1.0f / radius * 5.0f,
            .Quadratic = 1.0f / (radius * radius / 100.0f)
        };
    }

    Attenuation OmniLight::GetAttenuation() const
    {
        return attenuation;
    }

    SpotLight::SpotLight()
    {
        SetBlurAngles(15.0f, 25.0f);
    }

    void SpotLight::Dispatch(ObjVisitor &visitor)
    {
        visitor.VisitSpotLight(this);
    }

    void SpotLight::SetBlurAngles(float innerDeg, float outerDeg)
    {
        innerBlur = cos(glm::radians(innerDeg));
        outerBlur = cos(glm::radians(outerBlur));
    }

    float SpotLight::GetInnerBlur() const
    {
        return innerBlur;
    }

    float SpotLight::GetOuterBlur() const
    {
        return outerBlur;
    }
}

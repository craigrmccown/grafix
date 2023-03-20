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

    std::shared_ptr<OmniLight> OmniLight::Create()
    {
        return std::shared_ptr<OmniLight>(new OmniLight());
    }

    OmniLight::OmniLight()
    {
        // TODO: Normalize distance units
        SetRadius(50.0f);
    }

    void OmniLight::Dispatch(NodeVisitor &visitor)
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

    std::shared_ptr<SpotLight> SpotLight::Create()
    {
        return std::shared_ptr<SpotLight>(new SpotLight());
    }

    SpotLight::SpotLight()
    {
        SetBlurAngles(glm::radians(15.0f), glm::radians(25.0f));
    }

    void SpotLight::Dispatch(NodeVisitor &visitor)
    {
        visitor.VisitSpotLight(this);
    }

    void SpotLight::SetBlurAngles(float inner, float outer)
    {
        innerBlur = cos(inner);
        outerBlur = cos(outer);
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

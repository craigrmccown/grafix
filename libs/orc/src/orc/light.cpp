#include "light.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    Light::Light()
        : color(glm::vec3(1.0f))
        , phong(Phong{.Ambient = 0.05f, .Diffuse = 0.5f, .Specular = 0.8f})
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

    OmniLight::OmniLight() : brightness(1.25f) {}

    void OmniLight::Dispatch(NodeVisitor &visitor)
    {
        visitor.VisitOmniLight(this);
    }

    void OmniLight::SetBrightness(float brightness)
    {
        this->brightness = brightness;
    }

    float OmniLight::GetBrightness() const
    {
        return brightness;
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

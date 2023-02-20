#include "obj.hpp"
#include "types.hpp"

namespace orc
{
    class Light : public Obj
    {
        public:
        Light();

        void SetColor(float r, float g, float b);

        void SetPhong(float ambient, float diffuse, float specular);

        glm::vec3 GetColor() const;

        Phong GetPhong() const;

        private:
        glm::vec3 color;
        Phong phong;
    };

    class OmniLight : public Light
    {
        public:
        OmniLight();

        void SetRadius(float radius);

        Attenuation GetAttenuation() const;

        private:
        Attenuation attenuation;
    };

    class SpotLight : public Light
    {
        public:
        SpotLight();

        void SetBlurAngles(float innerDeg, float outerDeg);

        float GetInnerBlur() const;

        float GetOuterBlur() const;

        private:
        float innerBlur, outerBlur;
    };
}

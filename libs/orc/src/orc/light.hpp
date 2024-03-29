#pragma once

#include <memory>
#include "object.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Light : public Object
    {
        public:
        void SetColor(float r, float g, float b);

        void SetPhong(float ambient, float diffuse, float specular);

        glm::vec3 GetColor() const;

        Phong GetPhong() const;

        protected:
        Light();

        private:
        glm::vec3 color;
        Phong phong;
    };

    class OmniLight : public Light
    {
        public:
        static std::shared_ptr<OmniLight> Create();

        void Dispatch(NodeVisitor &visitor) override;

        void SetBrightness(float brightness);

        float GetBrightness() const;

        protected:
        OmniLight();

        private:
        float brightness;
    };

    class SpotLight : public Light
    {
        public:
        static std::shared_ptr<SpotLight> Create();

        void Dispatch(NodeVisitor &visitor) override;

        void SetBlurAngles(float innerDeg, float outerDeg);

        float GetInnerBlur() const;

        float GetOuterBlur() const;

        protected:
        SpotLight();

        private:
        float innerBlur, outerBlur;
    };
}

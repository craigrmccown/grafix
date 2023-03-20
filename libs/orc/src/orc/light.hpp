#pragma once

#include <memory>
#include "node.hpp"
#include "types.hpp"
#include "visitor.hpp"

namespace orc
{
    class Light : public Node
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

        void SetRadius(float radius);

        Attenuation GetAttenuation() const;

        protected:
        OmniLight();

        private:
        Attenuation attenuation;
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

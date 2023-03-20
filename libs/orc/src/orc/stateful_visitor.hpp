#pragma once

#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "regular.hpp"
#include "visitor.hpp"

namespace orc
{
    class StatefulVisitor : public NodeVisitor
    {
        public:
        void VisitCamera(Camera *camera) override;
        void VisitOmniLight(OmniLight *light) override;
        void VisitSpotLight(SpotLight *light) override;
        void VisitRegular(Regular *regular) override;

        const std::vector<Camera *> &GetCameras() const;
        const std::vector<OmniLight *> &GetOmniLights() const;
        const std::vector<SpotLight *> &GetSpotLights() const;
        const std::vector<Regular *> &GetRegulars() const;

        private:
        std::vector<Camera *> cameras;
        std::vector<OmniLight *> omniLights;
        std::vector<SpotLight *> spotLights;
        std::vector<Regular *> regulars;
    };
}

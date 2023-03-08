#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "regular.hpp"
#include "stateful_visitor.hpp"

namespace orc
{
    void StatefulVisitor::VisitCamera(Camera *camera)
    {
        cameras.push_back(camera);
    }

    void StatefulVisitor::VisitOmniLight(OmniLight *light)
    {
        omniLights.push_back(light);
    }

    void StatefulVisitor::VisitSpotLight(SpotLight *light)
    {
        spotLights.push_back(light);
    }

    void StatefulVisitor::VisitRegular(Regular *regular)
    {
        regulars.push_back(regular);
    }

    const std::vector<Camera *> &StatefulVisitor::GetCameras() const
    {
        return cameras;
    }

    const std::vector<OmniLight *> &StatefulVisitor::GetOmniLights() const
    {
        return omniLights;
    }

    const std::vector<SpotLight *> &StatefulVisitor::GetSpotLights() const
    {
        return spotLights;
    }

    const std::vector<Regular *> &StatefulVisitor::GetRegulars() const
    {
        return regulars;
    }
}

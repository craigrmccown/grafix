#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
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

    void StatefulVisitor::VisitObject(Object *object)
    {
        objects.push_back(object);
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

    const std::vector<Object *> &StatefulVisitor::GetObjects() const
    {
        return objects;
    }
}

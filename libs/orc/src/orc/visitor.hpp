#pragma once

#include <vector>

namespace orc
{
    // Forward declarations of visited classes

    class Camera;
    class OmniLight;
    class SpotLight;
    class Object;

    class NodeVisitor
    {
        public:
        virtual void VisitCamera(Camera *camera) = 0;
        virtual void VisitOmniLight(OmniLight *light) = 0;
        virtual void VisitSpotLight(SpotLight *light) = 0;
        virtual void VisitObject(Object *object) = 0;
    };
}

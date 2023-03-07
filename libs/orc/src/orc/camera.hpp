#pragma once

#include <glm/glm.hpp>
#include "obj.hpp"

namespace orc
{
    class Camera : public Obj
    {
        public:
        Camera(float fieldOfView, float aspectRatio);

        // See Obj.ComputeMxs. Also computes the view-projection matrix.
        void ComputeMxs();

        // Returns the view-projection matrix that can be used to apply camera
        // perspective to all other objects
        glm::mat4 GetViewProjectionMx() const;

        private:
        const glm::mat4 projectionMx;
        glm::mat4 viewProjectionMx;

        glm::mat4 GetViewMx() const;
    };
}

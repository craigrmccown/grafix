#pragma once

#include <glm/glm.hpp>
#include "obj.hpp"

namespace orc
{
    class Camera : public Obj
    {
        public:
        Camera(float fieldOfView, float aspectRatio);

        void ComputeMxs(glm::mat4 parentMx);

        glm::mat4 GetViewProjectionMx() const;

        private:
        const glm::mat4 projectionMx;
        glm::mat4 viewProjectionMx;

        glm::mat4 GetViewMx() const;
    };
}

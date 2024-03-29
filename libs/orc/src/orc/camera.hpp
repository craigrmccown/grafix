#pragma once

#include <glm/glm.hpp>
#include "node.hpp"
#include "visitor.hpp"

namespace orc
{
    class Camera : public Node
    {
        public:
        static std::shared_ptr<Camera> Create();

        void Dispatch(NodeVisitor &visitor) override;

        // Sets the camera's field of view angle, expressed in radians. Defaults
        // to pi/4 (45 deg).
        void SetFieldOfView(float fieldOfView);

        // Sets the aspect ratio of the perspective projection. Usually this
        // should be set to the screen's width/height. Defaults to 16/9,
        // although this is rarely correct.
        void SetAspectRatio(float aspectRatio);

        // Sets the frustum distances of the camera. Distances that fall out of
        // this range will be clipped. Near distance must be a non-zero,
        // positive number
        void SetClippingDistance(float near, float far);

        // See Node.ComputeMxs. Also computes the view-projection matrix.
        void ComputeMxs() override;

        // Returns a matrix that performs a transformation from world space to
        // view space
        glm::mat4 GetViewMx() const;

        // Returns a matrix that performs a transformation from view space to
        // clip space
        glm::mat4 GetProjectionMx() const;

        // Returns the view-projection matrix that can be used to apply camera
        // perspective to all other nodes
        glm::mat4 GetViewProjectionMx() const;

        protected:
        Camera();

        private:
        glm::mat4 viewMx, projectionMx, viewProjectionMx;
        float fieldOfView, aspectRatio, nearClip, farClip;

        void ComputeViewMx();
        void ComputeProjectionMx();
    };
}

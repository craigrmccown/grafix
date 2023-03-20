#pragma once

#include <glm/glm.hpp>
#include "node.hpp"
#include "visitor.hpp"

namespace orc
{
    class Camera : public Node
    {
        public:
        Camera();

        void Dispatch(NodeVisitor &visitor) override;

        void SetPerspective(float fieldOfView, float aspectRatio);

        // See Node.ComputeMxs. Also computes the view-projection matrix.
        void ComputeMxs() override;

        // Returns the view-projection matrix that can be used to apply camera
        // perspective to all other nodes
        glm::mat4 GetViewProjectionMx() const;

        private:
        glm::mat4 projectionMx;
        glm::mat4 viewProjectionMx;

        glm::mat4 GetViewMx() const;
    };
}

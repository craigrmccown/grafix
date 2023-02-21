#pragma once

#include <glm/glm.hpp>

namespace orc
{
    // The base class for drawable objects. Handles logic to position objects in
    // 3D space.
    class Obj
    {
        public:
        // Creates a new object at the origin facing the +Z direction
        Obj();

        // Moves the object in world space by the specified deltas along each
        // axis. This operation has no effect until ComputeMxs is called.
        void Translate(float x, float y, float z);

        // Rotates the object by the specified yaw, pitch, and roll angles,
        // expressed in radians. This operation has no effect until ComputeMxs
        // is called.
        void Rotate(float yaw, float pitch, float roll);

        // Computes the model matrix based on the current translation and
        // rotation. A parent matrix can be passed to transform the origin of
        // this operation. For example, if two objects should be spatially
        // grouped together so that the movement of one object should apply to
        // the other, the model matrix of the parent object can be passed when
        // computing the child's model matrix. An identity matrix can be passed
        // to compute a matrix relative to the world's origin.
        virtual void ComputeMxs(const glm::mat4 &parentMx);

        // Returns the model matrix produced by the last invocation of
        // ComputeMxs
        glm::mat4 GetModelMx() const;

        // Returns a unit vector pointing in the direction of this object. If
        // this object has not been rotated, the front vector aligns with the +Z
        // axis.
        glm::vec3 GetFront() const;

        // Returns a unit vector pointing in a direction orthogonal to the
        // direction of this object. If this object has not been rotated, the
        // right vector aligns with the +X axis.
        glm::vec3 GetRight() const;

        // Returns a unit vector pointing in a direction orthogonal to the
        // direction of this object. If this object has not been rotated, the
        // up vector aligns with the +Y axis.
        glm::vec3 GetUp() const;

        // Returns the position of this object, in world coordinate space. All
        // objects are initialized at the origin.
        glm::vec3 GetPosition() const;

        private:
        glm::vec3 translation;
        glm::vec3 rotation;
        glm::mat4 modelMx;
    };
}

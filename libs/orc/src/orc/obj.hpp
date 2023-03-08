#pragma once

#include <memory>
#include <set>
#include <glm/glm.hpp>
#include "visitor.hpp"

namespace orc
{
    // The base class for objects that exist in 3D space. Handles positioning
    // math and implements spatial hierarchy.
    class Obj : public std::enable_shared_from_this<Obj>
    {
        public:
        // Creates a new object at the origin facing the +Z direction
        Obj();

        // Implements the visitor pattern. Each concrete subclass of Obj should
        // override this method to dispatch the correct request to the visitor.
        virtual void Dispatch(ObjVisitor &visitor) = 0;

        // Moves the object in world space by the specified deltas along each
        // axis. This operation has no effect until ComputeMxs is called.
        void Translate(float x, float y, float z);

        // Rotates the object by the specified yaw, pitch, and roll angles,
        // expressed in radians. This operation has no effect until ComputeMxs
        // is called.
        void Rotate(float yaw, float pitch, float roll);

        // Computes the model matrix based on the current translation and
        // rotation. If attached, calculations are relative to the orientation
        // of the parent. If detached, they are relative to the world's origin.
        virtual void ComputeMxs();

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

        // Establishes a parent-child relationship between this Obj and the
        // specified child. Assumes shared ownership of the child. All child
        // transformations will be relative to its parent.
        void AttachChild(std::shared_ptr<Obj> child);

        // Detaches a node from another. Detached nodes aren't drawn by the
        // Scene, but can still be manipulated and reattached at a later point.
        void Detach();

        // Returns an Obj's children
        const std::set<std::shared_ptr<Obj>> &GetChildren() const;

        private:
        glm::vec3 translation;
        glm::vec3 rotation;
        glm::mat4 modelMx;
        std::set<std::shared_ptr<Obj>> children;
        std::weak_ptr<Obj> parent;
        bool isAttached;
    };
}

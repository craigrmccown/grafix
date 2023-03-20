#pragma once

#include <memory>
#include <set>
#include <glm/glm.hpp>
#include "visitor.hpp"

namespace orc
{
    // The base class for points that exist in 3D space. Handles positioning
    // math and implements spatial hierarchy.
    //
    // TODO: Support scaling transforms
    // TODO: Node type that participates in tranformation, but cannot be rendered
    class Node : public std::enable_shared_from_this<Node>
    {
        public:
        // Creates a new node at the origin facing the -Z direction
        Node();

        // Implements the visitor pattern. Each concrete subclass of Node should
        // override this method to dispatch the correct request to the visitor.
        virtual void Dispatch(NodeVisitor &visitor) = 0;

        // Moves the node relative to the coordinate system of its parent by the
        // specified deltas along each axis. This operation has no effect until
        // ComputeMxs is called.
        void Translate(float x, float y, float z);

        // Sets the translation of this node relative to the coordinate system
        // of its parent. This operation has no effect until ComputeMxs is
        // called.
        void SetTranslation(float x, float y, float z);

        // Rotates the node by the specified yaw, pitch, and roll angles,
        // expressed in radians. This operation has no effect until ComputeMxs
        // is called.
        void Rotate(float yaw, float pitch, float roll);

        // Sets the rotation of this node relative to the orientation of its
        // parent. This operation has no effect until ComputeMxs is called.
        void SetRotation(float yaw, float pitch, float roll);

        // Computes the model matrix based on the current translation and
        // rotation. If attached, calculations are relative to the orientation
        // of the parent. If detached, they are relative to the world's origin.
        virtual void ComputeMxs();

        // Returns the model matrix produced by the last invocation of
        // ComputeMxs
        glm::mat4 GetModelMx() const;

        // Returns a unit vector pointing in the direction of this node. If this
        // this node has not been rotated, the front vector aligns with the -Z
        // axis.
        glm::vec3 GetFront() const;

        // Returns a unit vector pointing in a direction orthogonal to the
        // direction of this node. If this node has not been rotated, the right
        // vector aligns with the +X axis.
        glm::vec3 GetRight() const;

        // Returns a unit vector pointing in a direction orthogonal to the
        // direction of this node. If this node has not been rotated, the up
        // vector aligns with the +Y axis.
        glm::vec3 GetUp() const;

        // Returns the position of this node, in world coordinate space. All
        // nodes are initialized at the origin.
        glm::vec3 GetPosition() const;

        // Establishes a parent-child relationship between this Node and the
        // specified child. Assumes shared ownership of the child. All child
        // transformations will be relative to its parent.
        void AttachChild(std::shared_ptr<Node> child);

        // Detaches a node from another. Detached nodes aren't drawn by the
        // Scene, but can still be manipulated and reattached at a later point.
        void Detach();

        // Returns a Node's children
        const std::set<std::shared_ptr<Node>> &GetChildren() const;

        private:
        glm::vec3 translation;
        glm::vec3 rotation;
        glm::mat4 modelMx;
        std::set<std::shared_ptr<Node>> children;
        std::weak_ptr<Node> parent;
        bool isAttached;
    };
}

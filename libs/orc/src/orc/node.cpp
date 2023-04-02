#define _USE_MATH_DEFINES

#include <cmath>
#include <memory>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "node.hpp"
#include "visitor.hpp"

// By convention, the default direction faces the -Z axis
const glm::vec4 worldFront(0.0f, 0.0f, -1.0f, 0.0f);
const glm::vec4 worldRight(1.0f, 0.0f, 0.0f, 0.0f);
const glm::vec4 worldUp(0.0f, 1.0f, 0.0f, 0.0f);
const glm::vec4 worldOrigin(0.0f, 0.0f, 0.0f, 1.0f);

static float modRadians(float rad)
{
    return remainderf(rad, 2 * M_PI);
}

static int getSign(float val)
{
    return (val > 0) - (val < 0);
}

namespace orc
{
    std::shared_ptr<Node> Node::Create()
    {
        return std::shared_ptr<Node>(new Node());
    }

    Node::Node()
        : translation(glm::vec3(0.0f))
        , rotation(glm::vec3(0.0f))
        , modelMx(glm::mat4(1.0f))
        , isAttached(false)
        {}

    void Node::Dispatch(NodeVisitor &visitor)
    {
        // No-op
    }

    void Node::Translate(float x, float y, float z)
    {
        translation += glm::vec3(x, y, z);
    }

    void Node::SetTranslation(float x, float y, float z)
    {
        translation = glm::vec3(x, y, z);
    }

    void Node::Rotate(float yaw, float pitch, float roll)
    {
        rotation += glm::vec3(yaw, pitch, roll);
        rotation.x = modRadians(rotation.x);
        rotation.y = modRadians(rotation.y);
        rotation.z = modRadians(rotation.z);
    }

    void Node::SetRotation(float yaw, float pitch, float roll)
    {
        // TODO: Represent rotation using quaternions
        rotation = glm::vec3(yaw, pitch, roll);
    }

    void Node::SetTransformMx(glm::mat4 mx)
    {
        translation = mx * worldOrigin;
        glm::extractEulerAngleYXZ(mx, rotation.x, rotation.y, rotation.z);
    }

    void Node::ComputeMxs()
    {
        glm::mat4 parentMx(1.0f);
        if (std::shared_ptr<Node> p = parent.lock())
        {
            parentMx = p->GetModelMx();
        }

        modelMx = parentMx *
            glm::translate(glm::mat4(1.0f), translation) *
            glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
    }

    glm::mat4 Node::GetModelMx() const
    {
        return modelMx;
    }

    glm::vec3 Node::GetFront() const
    {
        return GetModelMx() * worldFront;
    }

    glm::vec3 Node::GetRight() const
    {
        return GetModelMx() * worldRight;
    }

    glm::vec3 Node::GetUp() const
    {
        return GetModelMx() * worldUp;
    }

    glm::vec3 Node::GetPosition() const
    {
        return GetModelMx() * worldOrigin;
    }

    void Node::AttachChild(std::shared_ptr<Node> child)
    {
        if (child->isAttached)
        {
            throw std::logic_error("Child is already attached to another node");
        }

        child->parent = shared_from_this();
        children.insert(child);
        child->isAttached = true;
    }

    void Node::Detach()
    {
        if (std::shared_ptr<Node> p = parent.lock())
        {
            parent.reset();
            p->children.erase(shared_from_this());
            isAttached = false;
        }
    }

    const std::set<std::shared_ptr<Node>> &Node::GetChildren() const
    {
        return children;
    }
}

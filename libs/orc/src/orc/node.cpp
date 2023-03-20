#define _USE_MATH_DEFINES

#include <cmath>
#include <memory>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "node.hpp"
#include "visitor.hpp"

static float modRadians(float rad)
{
    return remainderf(rad, 2 * M_PI);
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
        rotation = glm::vec3(yaw, pitch, roll);
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
        // By convention, the default direction faces the -Z axis
        return GetModelMx() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    }

    glm::vec3 Node::GetRight() const
    {
        return GetModelMx() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Node::GetUp() const
    {
        return GetModelMx() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Node::GetPosition() const
    {
        return GetModelMx() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

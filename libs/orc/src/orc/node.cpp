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
        , scale(glm::vec3(1.0f))
        , modelMx(glm::mat4(1.0f))
        , isAttached(false)
        {}

    void Node::Dispatch(NodeVisitor &visitor)
    {
        // No-op
    }

    void Node::Translate(float x, float y, float z)
    {
        SetTranslation(translation.x + x, translation.y + y, translation.z + z);

    }

    void Node::SetTranslation(float x, float y, float z)
    {
        translation = glm::vec3(x, y, z);
    }

    void Node::Rotate(float yaw, float pitch, float roll)
    {
        SetRotation(
            modRadians(rotation.x + yaw),
            modRadians(rotation.y + pitch),
            modRadians(rotation.z + roll)
        );
    }

    void Node::SetRotation(float yaw, float pitch, float roll)
    {
        // TODO: Represent rotation using quaternions
        rotation = glm::vec3(yaw, pitch, roll);
    }

    void Node::Scale(float x, float y, float z)
    {
        SetScale(scale.x * x, scale.y * y, scale.z * z);
    }

    void Node::SetScale(float x, float y, float z)
    {
        if (x <= 0 || y <= 0 || z <= 0)
        {
            throw std::logic_error("Scaling factors must be positive and non-zero");
        }

        scale = glm::vec3(x, y, z);
    }

    // Assumes a T*R*S transformation matrix
    void Node::SetTransformMx(glm::mat4 mx)
    {
        // Translation vector can be extracted from the last column of the
        // matrix
        translation = glm::vec3(mx[3][0], mx[3][1], mx[3][2]);

        // Since each axis is be expressed as a unit vector in the pure rotation
        // matrix, the scaling factors can be derived by taking the magnitude
        // of each axis in the transformation matrix
        scale = glm::vec3(
            glm::length(glm::vec3(mx[0][0], mx[0][1], mx[0][2])),
            glm::length(glm::vec3(mx[1][0], mx[1][1], mx[1][2])),
            glm::length(glm::vec3(mx[2][0], mx[2][1], mx[2][2]))
        );

        // Construct a pure rotation matrix by dividing each scaled axis by its
        // respective scaling factor
        glm::mat4 pureRotationMx(1.0f);
        pureRotationMx[0][0] = mx[0][0] / scale.x;
        pureRotationMx[0][1] = mx[0][1] / scale.x;
        pureRotationMx[0][2] = mx[0][2] / scale.x;
        pureRotationMx[1][0] = mx[1][0] / scale.y;
        pureRotationMx[1][1] = mx[1][1] / scale.y;
        pureRotationMx[1][2] = mx[1][2] / scale.y;
        pureRotationMx[2][0] = mx[2][0] / scale.z;
        pureRotationMx[2][1] = mx[2][1] / scale.z;
        pureRotationMx[2][2] = mx[2][2] / scale.z;
        glm::extractEulerAngleYXZ(pureRotationMx, rotation.x, rotation.y, rotation.z);
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
            glm::yawPitchRoll(rotation.x, rotation.y, rotation.z) *
            glm::scale(glm::mat4(1.0f), scale);
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

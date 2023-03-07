#include <memory>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "obj.hpp"

namespace orc
{
    Obj::Obj()
        : translation(glm::vec3(0.0f))
        , rotation(glm::vec3(0.0f))
        , modelMx(glm::mat4(1.0f))
        , isAttached(false)
        {}

    void Obj::Translate(float x, float y, float z)
    {
        translation += glm::vec3(x, y, z);
    }

    void Obj::Rotate(float yaw, float pitch, float roll)
    {
        // TODO: Divide by 2pi radians to prevent overflow
        rotation += glm::vec3(yaw, pitch, roll);
    }

    void Obj::ComputeMxs()
    {
        glm::mat4 parentMx(1.0f);
        if (std::shared_ptr<Obj> p = parent.lock())
        {
            parentMx = p->GetModelMx();
        }

        modelMx = parentMx *
            glm::translate(glm::mat4(1.0f), translation) *
            glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
    }

    glm::mat4 Obj::GetModelMx() const
    {
        return modelMx;
    }

    glm::vec3 Obj::GetFront() const
    {
        return GetModelMx() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    glm::vec3 Obj::GetRight() const
    {
        return GetModelMx() * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 Obj::GetUp() const
    {
        return GetModelMx() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    glm::vec3 Obj::GetPosition() const
    {
        return GetModelMx() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void Obj::AttachChild(std::shared_ptr<Obj> child)
    {
        if (child->isAttached)
        {
            throw std::logic_error("Child is already attached to another object");
        }

        child->parent = shared_from_this();
        children.insert(child);
        child->isAttached = true;
    }

    void Obj::Detach()
    {
        if (std::shared_ptr<Obj> p = parent.lock())
        {
            parent.reset();
            p->children.erase(shared_from_this());
            isAttached = false;
        }
    }

    const std::set<std::shared_ptr<Obj>> &Obj::GetChildren() const
    {
        return children;
    }
}

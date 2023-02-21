#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "obj.hpp"

namespace orc
{
    Obj::Obj()
        : translation(glm::vec3(0.0f))
        , rotation(glm::vec3(0.0f))
        , modelMx(glm::mat4(1.0f))
        {}

    void Obj::Translate(float x, float y, float z)
    {
        translation += glm::vec3(x, y, z);
    }

    void Obj::Rotate(float yaw, float pitch, float roll)
    {
        rotation += glm::vec3(yaw, pitch, roll);
    }

    void Obj::ComputeMxs(glm::mat4 parentMx)
    {
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
}

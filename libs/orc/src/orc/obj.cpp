#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "obj.hpp"

namespace orc
{
    Obj::Obj()
        : pendingTranslation(glm::vec3(0.0f))
        , pendingRotation(glm::vec3(0.0f))
        , localMx(glm::mat4(1.0f))
        , worldMx(glm::mat4(1.0f))
        {}

    void Obj::Translate(float x, float y, float z)
    {
        pendingTranslation += glm::vec3(x, y, z);
    }

    void Obj::Rotate(float yaw, float pitch, float roll)
    {
        pendingRotation += glm::vec3(yaw, pitch, roll);
    }

    void Obj::ComputeMxs(glm::mat4 parentMx)
    {
        glm::mat4 transformMx = glm::yawPitchRoll(pendingRotation.x, pendingRotation.y, pendingRotation.z);
        transformMx = glm::translate(transformMx, pendingTranslation);

        localMx = transformMx * localMx;
        worldMx = parentMx * localMx;
        pendingTranslation = pendingRotation = glm::vec3(0.0f);
    }

    glm::mat4 Obj::GetModelMx() const
    {
        return worldMx;
    }

    glm::vec3 Obj::GetFront() const
    {
        return GetModelMx() * glm::vec4(0.0, 0.0, 1.0, 0.0);
    }

    glm::vec3 Obj::GetRight() const
    {
        return GetModelMx() * glm::vec4(0.1, 0.0, 0.0, 0.0);
    }

    glm::vec3 Obj::GetUp() const
    {
        return GetModelMx() * glm::vec4(0.0, 0.1, 0.0, 0.0);
    }

    glm::vec3 Obj::GetPosition() const
    {
        return GetModelMx() * glm::vec4(0.0, 0.0, 0.0, 1.0);
    }
}

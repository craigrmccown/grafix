#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"

namespace orc
{
    Camera::Camera(float fieldOfView, float aspectRatio)
        : projectionMx(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.0f, 100.0f))
        {}

    void Camera::ComputeMxs(glm::mat4 parentMx)
    {
        Obj::ComputeMxs(parentMx);
        viewProjectionMx = projectionMx * GetViewMx();
    }

    glm::mat4 Camera::GetViewProjectionMx() const {
        return viewProjectionMx;
    }

    glm::mat4 Camera::GetViewMx() const
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 dir = GetFront();
        glm::vec3 up = GetUp();

        return glm::lookAt(pos, pos + dir, up);
    }
}

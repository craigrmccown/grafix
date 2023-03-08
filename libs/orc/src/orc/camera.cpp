#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "visitor.hpp"

void copyVecToRow(const glm::vec3 &v, glm::mat4 &m, int r)
{
    for (int c = 0; c < v.length(); c++)
    {
        m[c][r] = v[c];
    }
}

namespace orc
{
    Camera::Camera(float fieldOfView, float aspectRatio)
        : projectionMx(glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.0f, 100.0f)) // TODO: Configurable frustum distances
    {
        ComputeMxs();
    }

    void Camera::Dispatch(ObjVisitor &visitor)
    {
        visitor.VisitCamera(this);
    }

    void Camera::ComputeMxs()
    {
        Obj::ComputeMxs();
        viewProjectionMx = projectionMx * GetViewMx();
    }

    glm::mat4 Camera::GetViewProjectionMx() const {
        return viewProjectionMx;
    }

    glm::mat4 Camera::GetViewMx() const
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 up = GetUp();
        glm::vec3 front = GetFront();
        glm::vec3 right = GetRight();

        // Conceptually, we construct the view matrix by inverting the camera's
        // model matrix, which is equivalent to transposing the rotation matrix
        // and negating the translation matrix, then multiplying them. The
        // operations below are doing the same thing, but we take a shortcut by
        // putting each element directly in place instead of actually performing
        // matrix multiplication.
        glm::mat4 viewMx(1.0f);
        copyVecToRow(right, viewMx, 0);
        copyVecToRow(up, viewMx, 1);
        copyVecToRow(front, viewMx, 2);
        viewMx[3][0] = -glm::dot(right, pos);
        viewMx[3][1] = -glm::dot(up, pos);
        viewMx[3][2] = -glm::dot(front, pos);

        return viewMx;
    }
}

#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "visitor.hpp"

const float
    defaultFieldOfView = glm::radians(45.0f),
    defaultAspectRatio = 16.0f/9.0f,
    defaultNearClip = 1.0f,
    defaultFarClip = 200.0f;

static void copyVecToRow(const glm::vec3 &v, glm::mat4 &m, int r)
{
    for (int c = 0; c < v.length(); c++)
    {
        m[c][r] = v[c];
    }
}

namespace orc
{
    std::shared_ptr<Camera> Camera::Create()
    {
        return std::shared_ptr<Camera>(new Camera());
    }

    Camera::Camera()
        : fieldOfView(defaultFieldOfView)
        , aspectRatio(defaultAspectRatio)
        , nearClip(defaultNearClip)
        , farClip(defaultFarClip)
    {
        ComputeMxs();
    }

    void Camera::Dispatch(NodeVisitor &visitor)
    {
        visitor.VisitCamera(this);
    }

    void Camera::SetFieldOfView(float fieldOfView)
    {
        this->fieldOfView = fieldOfView;
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        this->aspectRatio = aspectRatio;
    }

    void Camera::SetClippingDistance(float near, float far)
    {
        if (near <= 0)
        {
            throw std::logic_error("Near clipping distance must be greater than 0");
        }

        nearClip = near;
        farClip = far;
    }

    void Camera::ComputeMxs()
    {
        Node::ComputeMxs();
        ComputeViewMx();
        ComputeProjectionMx();
        viewProjectionMx = projectionMx * viewMx;
    }

    glm::mat4 Camera::GetViewMx() const
    {
        return viewMx;
    }

    glm::mat4 Camera::GetProjectionMx() const
    {
        return projectionMx;
    }

    glm::mat4 Camera::GetViewProjectionMx() const
    {
        return viewProjectionMx;
    }

    void Camera::ComputeViewMx()
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 up = GetUp();

        // By convention, the camera faces the -Z direction by default (right
        // handed coordinate system). When converting world space to view space,
        // we want to transform the +Z axis of one system to the +Z axis of the
        // other. Since the +Z axis of the camera's coordinate system actually
        // points toward/behind the camera, we negate the front vector.
        glm::vec3 front = -GetFront();
        glm::vec3 right = GetRight();

        // Conceptually, the view matrix converts world space to view space by
        // translating everything in the opposite direction of its position so
        // that the camera's position becomes the new origin. Then, everything
        // is rotated around that origin in the opposite direction of the
        // camera. So, if the camera moves backward and looks up, in reality
        // we just move every thing forward and rotate it down. Mathematically,
        // we negate the camera's position to form a matrix T, transpose the
        // camera's rotation matrix to form a matrix R, then calculate R * T
        // (translate, then rotate). The operations below are doing exactly
        // that, but we take a shortcut by putting each element directly in
        // place instead of actually performing matrix multiplication.
        viewMx = glm::mat4(1.0f);
        copyVecToRow(right, viewMx, 0);
        copyVecToRow(up, viewMx, 1);
        copyVecToRow(front, viewMx, 2);
        viewMx[3][0] = -glm::dot(right, pos);
        viewMx[3][1] = -glm::dot(up, pos);
        viewMx[3][2] = -glm::dot(front, pos);
    }

    void Camera::ComputeProjectionMx()
    {
        projectionMx = glm::perspective(fieldOfView, aspectRatio, nearClip, farClip);
    }
}

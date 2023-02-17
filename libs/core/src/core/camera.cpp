#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "controls.hpp"

namespace constants
{
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const float speed = 6.0f, maxPitchDeg = 89.0f;
}

Camera::Camera(glm::vec3 initPos)
    : pos(initPos)
    , dir(glm::vec3(0.0f, 0.0f, 1.0f))
    , yaw(0.0f)
    , pitch(0.0f)
    , view(glm::mat4(1.0f))
    {}

// TODO: Avoid unnecessary work if no input is received
void Camera::move(glm::vec2 translate, glm::vec2 rotate)
{
    // Mouse coordinates use inverted y-axis, so multiply by -1
    incPitch(rotate.y);
    incYaw(rotate.x);

    // Compute a unit vector that points in the direction of the camera.
    dir = glm::vec3(sin(glm::radians(yaw)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)));

    // We can set the y-component to 0 if we want to stay on the ground. Setting
    // the front vector equal to the look direction gives us a "fly" style
    // camera.
    glm::vec3 front(dir.x, dir.y, dir.z);

    // The right vector is always orthoganal to the front and up vectors
    glm::vec3 right = glm::cross(front, constants::up);

    // Compute a vector representing the direction and magnitude of camera
    // movement. Movement is scaled up by the camera's speed.
    glm::vec3 translation = (front * translate.y + right * translate.x) * constants::speed;
    pos += translation;
}

void Camera::incYaw(float delta)
{
    // Use modular arithmetic to cycle yaw to prevent eventual overflow
    yaw += delta;
    int whole = yaw;
    yaw = whole % 360 + yaw - whole;
}

void Camera::incPitch(float delta)
{
    // Clamp pitch between reasonable values to avoid vertical flipping
    pitch += delta;
    if (pitch > constants::maxPitchDeg) pitch = constants::maxPitchDeg;
    else if (pitch < -constants::maxPitchDeg) pitch = -constants::maxPitchDeg;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(pos, pos + dir, constants::up);
}

glm::vec3 Camera::getPosition()
{
    return pos;
}

glm::vec3 Camera::getDirection()
{
    return dir;
}

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "clock.hpp"
#include "camera.hpp"

namespace constants
{
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const float speed = 3.0f, hSensitivity = 0.2f, vSensitivity = 0.08f, maxPitchDeg = 89.0f;
}

Camera::Camera(Clock &clock, glm::vec3 initPos)
    : clock(clock)
    , pos(initPos)
    , yaw(0.0f)
    , pitch(0.0f)
    , view(glm::mat4(1.0f))
    {}

// TODO: Avoid unnecessary work if no input is received
void Camera::processInput(GLFWwindow *window)
{
    glm::vec2 mouseDelta = clock.getMouseDelta();

    // Mouse coordinates use inverted y-axis, so multiply by -1
    incPitch(-mouseDelta.y * constants::vSensitivity);
    incYaw(mouseDelta.x * constants::hSensitivity);

    // Compute a unit vector that points in the direction of the camera.
    glm::vec3 lookDir(sin(glm::radians(yaw)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)));

    // We can set the y-component to 0 if we want to stay on the ground. Setting
    // the front vector equal to the look direction gives us a "fly" style
    // camera.
    glm::vec3 front(lookDir.x, lookDir.y, lookDir.z);

    // The right vector is always orthoganal to the front and up vectors
    glm::vec3 right = glm::cross(front, constants::up);

    // Compute a vector representing the direction of camera movement. We only care
    // about the direction component; magnitude of the translation will be computed
    // based on the clock value and movement speed.
    glm::vec3 moveDir(0.0f, 0.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;

    move(moveDir);
    view = glm::lookAt(pos, pos + lookDir, constants::up);
}

void Camera::move(glm::vec3 dir)
{
    // Early exit if vector has undefined direction
    if (dir.x == 0 && dir.y == 0 && dir.z == 0) return;

    // Convert to unit vector before applying movement speed
    dir = glm::normalize(dir);
    glm::vec3 velocity = dir * constants::speed;

    // Compute movement delta and apply to current position
    glm::vec3 translation = velocity * (float)clock.getElapsedSeconds();
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
    return view;
}

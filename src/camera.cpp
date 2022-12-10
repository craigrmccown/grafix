#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "clock.hpp"
#include "camera.hpp"

namespace constants
{
    // TODO: Recompute front and right vectors on each frame when camera rotation is
    // introduced
    const glm::vec3 front(0.0f, 0.0f, -1.0f), right(1.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
    const float speed = 3.0f;
}

Camera::Camera(Clock &clock, glm::vec3 initPos) : clock(clock), pos(initPos) {}

void Camera::processInput(GLFWwindow *window)
{
    // Compute a vector representing the direction of camera movement. We only care
    // about the direction component; magnitude of the translation will be computed
    // based on the clock value and movement speed.
    glm::vec3 dir(0.0f, 0.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir += constants::front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir -= constants::right;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir -= constants::front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir += constants::right;

    pan(dir);
}

void Camera::pan(glm::vec3 dir)
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

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(pos, pos + constants::front, constants::up);
}

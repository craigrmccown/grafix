#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace mouse
{
    // Should be called only once after glfwInit()
    void listenForMovement(GLFWwindow *window);

    // Returns a pointer to the last known mouse position or a vector containing NAN
    // if movement has not yet been detected
    const glm::vec2 getPosition();

    // Returns whether mouse movement has been detected. If false, a resulting call
    // to getMousePosition will return a vector containing NAN.
    const bool hasMoved();
}

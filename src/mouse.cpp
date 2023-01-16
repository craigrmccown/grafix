#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "mouse.hpp"

namespace mouse {
    static glm::vec2 mousePos(NAN);

    const bool hasMoved()
    {
        return !std::isnan(mousePos.x + mousePos.y);
    }

    void mouseListener(GLFWwindow* window, double x, double y)
    {
        mousePos.x = x;
        mousePos.y = y;
    }

    void listenForMovement(GLFWwindow *window)
    {
        // Avoid registering multiple callbacks, even if invoked multiple
        // times
        static bool isListening = false;
        if (isListening) return;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouseListener);

        isListening = true;
    }

    const glm::vec2 getPosition()
    {
        return mousePos;
    }
}

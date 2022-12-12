#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "mouse.hpp"

static glm::vec2 *mousePos;

void mouseListener(GLFWwindow* window, double x, double y)
{
    if (!mousePos)
    {
        mousePos = new glm::vec2;
    }
    mousePos->x = x;
    mousePos->y = y;
}

void listenForMouseMovement(GLFWwindow *window)
{
    // Avoid registering multiple callbacks, even if invoked multiple
    // times
    static bool isListening = false;
    if (isListening) return;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouseListener);

    isListening = true;
}

const glm::vec2 *getMousePosition()
{
    return mousePos;
}

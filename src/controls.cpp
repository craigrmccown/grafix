#include <cmath>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "controls.hpp"
#include "history_map.hpp"
#include "mouse.hpp"

Controls::Controls() : binaryState(false) {}

bool Controls::queryBinaryAction(BinaryAction action, BinaryActionState state)
{
    bool isOn = binaryState.get(action);
    switch (state)
    {
        case BinaryActionState::on:
            return isOn;
        case BinaryActionState::off:
            return !isOn;
        case BinaryActionState::leading:
            return isOn && !binaryState.getPrev(action);
        case BinaryActionState::trailing:
            return !isOn && binaryState.getPrev(action);
    }
}

glm::vec2 Controls::queryDirectionalAction(DirectionalAction action)
{
    return directionalState[action];
}

KeyboardMouseControls::KeyboardMouseControls(GLFWwindow &window, float sensitivity)
    : window(window)
    , sensitivity(sensitivity)
    , lastMousePos(NAN)
    {}

void KeyboardMouseControls::processInput(float elapsed)
{
    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        binaryState.set(BinaryAction::exit, true);
    }

    int
        wPress = glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS,
        aPress = glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS,
        sPress = glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS,
        dPress = glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS;

    // Pressing keys applies a constant velocity in the direction of the key
    // press. Convert to unit vector so that movement in a diagonal direction
    // does not result in a greater magnitude.
    glm::vec2 velocity(dPress - aPress, wPress - sPress);
    if (velocity.x != 0 || velocity.y != 0)
    {
        velocity = glm::normalize(velocity);
    }

    // Multiply velocity vector by elapsed time to get change in distance
    directionalState[DirectionalAction::primaryMove] = velocity * elapsed;

    // Short circuit if we haven't received any mouse movement
    if (!mouse::hasMoved()) return;

    // Start tracking mouse deltas only after we have received two mouse
    // positions
    glm::vec2 mousePos = getMousePos();
    if (!std::isnan(lastMousePos.x + lastMousePos.y))
    {
        directionalState[DirectionalAction::secondaryMove] = (mousePos - lastMousePos) * sensitivity;
    }
    lastMousePos = mousePos;
}

glm::vec2 KeyboardMouseControls::getMousePos()
{
    double mouseX, mouseY;
    glfwGetCursorPos(&window, &mouseX, &mouseY);

    // Mouse Y coordinate is inverted, so multiply by -1
    return glm::vec2(mouseX, -mouseY);
}

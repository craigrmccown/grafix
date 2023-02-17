#include <cmath>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "controls.hpp"
#include "history_map.hpp"
#include "mouse.hpp"

Controls::Controls() : signalState(0.0f) {}

double Controls::getValue(Controls::Signal signal)
{
    return signalState.get(signal);
}

bool Controls::isLeading(Controls::Signal signal)
{
    return signalState.get(signal) > 0 && signalState.getPrev(signal) == 0;
}

bool Controls::isTrailing(Controls::Signal signal)
{
    return signalState.get(signal) == 0 && signalState.getPrev(signal) > 0;
}

KeyboardMouseControls::KeyboardMouseControls(GLFWwindow &window, float sensitivity)
    : window(window)
    , sensitivity(sensitivity)
    , lastMousePos(NAN)
    {}

void KeyboardMouseControls::newFrame(float elapsed)
{
    signalState.set(Signal::exit, (float)(glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS));
    signalState.set(Signal::action1, (float)(glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS));
    signalState.set(Signal::action2, (float)(glfwGetKey(&window, GLFW_KEY_F) == GLFW_PRESS));

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
    glm::vec2 distance = velocity * elapsed;
    signalState.set(Signal::moveX, distance.x);
    signalState.set(Signal::moveY, distance.y);

    // Short circuit if we haven't received any mouse movement
    if (!mouse::hasMoved()) return;

    // Start tracking mouse deltas only after we have received two mouse
    // positions
    glm::vec2 mousePos = mouse::getPosition();
    if (!std::isnan(lastMousePos.x + lastMousePos.y))
    {
        glm::vec2 mouseDistance = (mousePos - lastMousePos) * sensitivity;
        signalState.set(Signal::aimX, mouseDistance.x);
        signalState.set(Signal::aimY, -mouseDistance.y);
    }
    lastMousePos = mousePos;
}

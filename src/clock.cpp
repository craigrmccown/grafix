#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "clock.hpp"
#include "mouse.hpp"

Clock::Clock() : hasTicked(false), elapsed(0.0), mouseDelta(0.0f, 0.0f) {}

void Clock::tick()
{
    double t = glfwGetTime();
    if (!hasTicked)
    {
        hasTicked = true;
    }
    else
    {
        // We can only calculate elapsed time after two or more ticks
        // have passed
        elapsed = t - lastTick;
    }
    lastTick = t;

    // Start keeping track of mouse deltas once we receive our first position
    const glm::vec2 *m = getMousePosition();
    if (m)
    {
        // Allocate space for storing the previous mouse position if we haven't
        // already, or calculate the mouse delta if we have
        if (!lastMousePos)
        {
            lastMousePos = std::make_unique<glm::vec2>(0.0f, 0.0f);
        }
        else
        {
            mouseDelta = *m - *lastMousePos;
        }
        memcpy(lastMousePos.get(), m, sizeof(glm::vec2));
    }
}

double Clock::getElapsedSeconds()
{
    return elapsed;
}

glm::vec2 Clock::getMouseDelta()
{
    return mouseDelta;
}

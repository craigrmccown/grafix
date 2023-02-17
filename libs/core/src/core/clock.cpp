#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "clock.hpp"

Clock::Clock() : hasTicked(false), elapsed(0.0) {}

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
}

double Clock::getElapsedSeconds()
{
    return elapsed;
}

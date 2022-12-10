#include <GLFW/glfw3.h>
#include "clock.hpp"

Clock::Clock() : hasTicked(false) {}

void Clock::tick()
{
    double t = glfwGetTime();

    // All time values are relative to the first tick
    if (!hasTicked)
    {
        hasTicked = true;
        lastTick = t;
    }

    elapsed = t - lastTick;
    lastTick = t;
}

double Clock::getElapsedSeconds()
{
    if (!hasTicked) return 0;
    return elapsed;
}

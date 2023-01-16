#pragma once

#include <memory>
#include <glm/glm.hpp>

class Clock
{
    public:
    Clock();

    // Should be called once on every frame
    void tick();

    // Gets time elapsed since last tick in seconds
    double getElapsedSeconds();

    private:
    bool hasTicked;
    double lastTick;
    double elapsed;
};

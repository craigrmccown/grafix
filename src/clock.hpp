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

    // Gets the mouse position delta since the last tick
    glm::vec2 getMouseDelta();

    private:
    // Timer values
    bool hasTicked;
    double lastTick;
    double elapsed;

    // Mouse values
    std::unique_ptr<glm::vec2> lastMousePos;
    glm::vec2 mouseDelta;
};

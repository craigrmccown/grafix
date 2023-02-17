#pragma once

#include <map>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "history_map.hpp"

/**
 * Abstracts the input device from the rest of the engine. Can be implemented
 * to support any input device, e.g. keyboard and mouse, controller, etc.
 */
class Controls
{
    public:
    enum class Signal {
        moveX,
        moveY,
        aimX,
        aimY,
        exit,
        action1,
        action2,
    };

    Controls();

    // Must be overridden. Should be called every frame with the number of
    // partial seconds that have elapsed since the previous call.
    virtual void newFrame(float elapsed) = 0;

    double getValue(Signal signal);

    bool isLeading(Signal signal);

    bool isTrailing(Signal signal);

    protected:
    HistoryMap<Signal, float> signalState;
};

class KeyboardMouseControls : public Controls
{
    public:
    // TODO: Define proper configuration struct, including horizontal/vertical
    // sensitivity, key mapping, etc.
    KeyboardMouseControls(GLFWwindow &window, float sensitivity);

    void newFrame(float elapsed);

    private:
    GLFWwindow &window;
    float sensitivity;
    glm::vec2 lastMousePos;
};

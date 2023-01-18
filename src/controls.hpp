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
    /**
     * Actions that are conceptually discrete and have an on/off state, like
     * button presses
     */
    enum class BinaryAction {
        exit,
        action1,
        action2,
    };

    /**
     * Every frame, each binary action can be in one of these states
     */
    enum class BinaryActionState {
        on, // The button is pressed
        off, // The button is not pressed
        leading, // The button was pressed this frame
        trailing, // The button was released this frame
    };

    /**
     * Actions that have a magnitudes along 2 axes, like tilting a joystick.
     * Directional actions are reported as deltas between frames, expressed as
     * distances.
     */
    enum class DirectionalAction {
        primaryMove,
        secondaryMove,
    };

    Controls();

    // Must be overridden. Should be called every frame with the number of
    // partial seconds that have elapsed since the previous call.
    virtual void processInput(float elapsed) = 0;

    // Returns whether a binary action is in a given state for the current frame
    bool queryBinaryAction(BinaryAction action, BinaryActionState state);

    // Returns the distance delta of a directional action since the last frame
    glm::vec2 queryDirectionalAction(DirectionalAction action);

    protected:
    // Subclasses should set the state of each binary action on every frame.
    // For example, if a button is pressed when processInput is called, the
    // implementation should call binaryState.set(true);
    HistoryMap<BinaryAction, bool> binaryState;

    // Subclasses should set the state of each directional action on every
    // frame. Directional actions are expressed as deltas. For example, if a
    // joystick is halfway tilted in a certain direction, the implementation
    // could express that as a distance by multiplying the tilt, sensitivity,
    // and time elapsed since the last frame. Units of distance are defined by
    // each implementation.
    std::map<DirectionalAction, glm::vec2> directionalState;
};

class KeyboardMouseControls : public Controls
{
    public:
    // TODO: Define proper configuration struct, including horizontal/vertical
    // sensitivity, key mapping, etc.
    KeyboardMouseControls(GLFWwindow &window, float sensitivity);

    void processInput(float elapsed);

    private:
    GLFWwindow &window;
    float sensitivity;
    glm::vec2 lastMousePos;

    glm::vec2 getMousePos();
};

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "clock.hpp"

class Camera
{
    public:
    Camera(Clock &clock, glm::vec3 initPos);

    // TODO: Create abstraction around input processing to allow for
    // generic subscription to input on each frame and decoupling from
    // low level window inputs
    void processInput(GLFWwindow *window);

    glm::mat4 getViewMatrix();

    private:
    Clock &clock;

    // Translated position from origin
    glm::vec3 pos;

    // Euler angles expressed in degrees
    float yaw;
    float pitch;

    // View transformation matrix that is recomputed when input is processed
    glm::mat4 view;

    void move(glm::vec3 dir);
    void incYaw(float delta);
    void incPitch(float delta);
};

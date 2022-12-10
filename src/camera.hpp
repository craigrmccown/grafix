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
    glm::vec3 pos;

    void pan(glm::vec3 dir);
};

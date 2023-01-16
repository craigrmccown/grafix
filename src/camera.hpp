#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "clock.hpp"
#include "controls.hpp"

class Camera
{
    public:
    Camera(Controls &ctrl, glm::vec3 initPos);

    void computeViewMatrix();

    glm::mat4 getViewMatrix();

    private:
    Controls &ctrl;

    // Translated position from origin
    glm::vec3 pos;

    // Euler angles expressed in degrees
    float yaw;
    float pitch;

    // View transformation matrix that is recomputed when input is processed
    glm::mat4 view;

    void incYaw(float delta);
    void incPitch(float delta);
};

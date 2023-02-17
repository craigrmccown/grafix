#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "clock.hpp"
#include "controls.hpp"

class Camera
{
    public:
    Camera(glm::vec3 initPos);

    void move(glm::vec2 translate, glm::vec2 rotate);

    glm::mat4 getViewMatrix();

    glm::vec3 getPosition();

    glm::vec3 getDirection();

    private:
    // Translated position from origin
    glm::vec3 pos;

    // Unit vector pointing in direction of camera
    glm::vec3 dir;

    // Euler angles expressed in degrees
    float yaw;
    float pitch;

    // View transformation matrix that is recomputed when input is processed
    glm::mat4 view;

    void incYaw(float delta);
    void incPitch(float delta);
};
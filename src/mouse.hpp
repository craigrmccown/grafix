#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void listenForMouseMovement(GLFWwindow *window);

// Returns a pointer to the last known mouse position or nullptr if mouse
// movement has not yet been detected
const glm::vec2 *getMousePosition();

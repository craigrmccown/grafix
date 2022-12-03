#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "image.hpp"
#include "rectangle.hpp"
#include "shader.hpp"

int kill(const char *message) {
    std::cerr << message << std::endl;
    glfwTerminate();
    return -1;
}

// Set OpenGL version and core profile
void initializeGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow *openWindowedFullscreenWindow(const char *title) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    return glfwCreateWindow(mode->width, mode->height, title, monitor, NULL);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

glm::mat4 rotateAndScale(float rotate, float scale) {
    // Begin with the identiy matrix
    glm::mat4 transform(1.0f);

    // Rotate around the z axis
    transform = glm::rotate(transform, rotate, glm::vec3(0, 0, 1));

    // Scale last to avoid scaling rotation
    return glm::scale(transform, glm::vec3(scale));
}

int main()
{
    initializeGLFW();

    GLFWwindow *window = openWindowedFullscreenWindow("Grafix Demo");
    if (window == NULL)
    {
        return kill("Failed to create window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return kill("Failed to initialize GLAD");
    }

    std::vector<ShaderSrc> srcs {
        loadShaderSrc("./src/shaders/shader.vert"),
        loadShaderSrc("./src/shaders/shader.frag"),
    };

    Shader shader;
    shader.build(srcs);

    Rectangle rectangle("./assets/textures/crate.png");
    rectangle.load();

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Compute transformation matrix
        float t = glfwGetTime();
        glm::mat4 transform = rotateAndScale(glfwGetTime(), (t - int(t)) / 2);

        // Fill background color first
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setUniformMat4("transform", transform);
        rectangle.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

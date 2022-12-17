#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "clock.hpp"
#include "image.hpp"
#include "model.hpp"
#include "mouse.hpp"
#include "shader.hpp"

// TODO: Load vertex data from model file
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

int kill(const char *message)
{
    std::cerr << message << std::endl;
    glfwTerminate();
    return -1;
}

// Set OpenGL version and core profile
void initializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

const GLFWvidmode *getVideoMode()
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    return glfwGetVideoMode(monitor);
}

GLFWwindow *openWindowedFullscreenWindow(const char *title)
{
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

int main()
{
    initializeGLFW();

    GLFWwindow *window = openWindowedFullscreenWindow("Grafix Demo");
    if (window == NULL)
    {
        return kill("Failed to create window");
    }
    glfwMakeContextCurrent(window);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return kill("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    listenForMouseMovement(window);

    Shader objShader, lightShader;
    objShader.build(std::vector<ShaderSrc> {
        loadShaderSrc("./src/shaders/obj.vert"),
        loadShaderSrc("./src/shaders/obj.frag"),
    });
    lightShader.build(std::vector<ShaderSrc> {
        loadShaderSrc("./src/shaders/light.vert"),
        loadShaderSrc("./src/shaders/light.frag"),
    });

    Model cube(
        std::vector<float>(std::begin(vertices), std::end(vertices)),
        "./assets/textures/crate.png"
    );
    cube.load();

    Clock clock;
    Camera camera(clock, glm::vec3(0.0f, 0.0f, 5.0f));

    glm::vec3 modelPos(0.0f);
    glm::vec3 lightPos(10.0f, 6.0f, 3.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // Initialize projection matrix outside of render loop. Use an arbitrary
    // 45 degree field of view
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window))
    {
        clock.tick();

        processInput(window);
        camera.processInput(window);

        // Compute projection-view matrix
        glm::mat4 pvm = projection * camera.getViewMatrix();

        // Fill background color first
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create model matrix and translate to camera perspective
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, modelPos);
        transform = pvm * transform;

        // Set obj shader uniforms and draw model
        objShader.use();
        objShader.setUniformMat4("transform", transform);
        objShader.setUniformVec3("lightColor", lightColor);
        cube.draw();

        // Create light matrix and reuse camera transformation
        transform = glm::mat4(1.0f);
        transform = glm::scale(transform, glm::vec3(0.2f));
        transform = glm::translate(transform, lightPos);
        transform = pvm * transform;

        // Set light shader uniforms and draw light
        lightShader.use();
        lightShader.setUniformMat4("transform", transform);
        cube.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

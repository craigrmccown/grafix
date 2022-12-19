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
    // vertex               // texture      // normal
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
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
    glm::vec3 lightPos(12.0f, 7.0f, 5.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // Initialize projection matrix outside of render loop. Use an arbitrary
    // 45 degree field of view
    glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window))
    {
        clock.tick();

        processInput(window);
        camera.processInput(window);
        glm::mat4 viewMat = camera.getViewMatrix();

        // Fill background color first
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create model matrix and transform to camera perspective
        glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), modelPos);
        glm::mat4 modelViewMat = viewMat * modelMat;
        glm::mat4 transformMat = projectionMat * modelViewMat;

        // Transform light position to view space - we use view space in the
        // shader for lighting calculations
        glm::vec3 viewLightPos(viewMat * glm::vec4(lightPos, 1.0f));

        // Set obj shader uniforms and draw model
        // TODO: Pass normal matrix instead of model matrix to account for
        // non-uniform scaling
        objShader.use();
        objShader.setUniformMat4("transformMat", transformMat);
        objShader.setUniformMat4("modelViewMat", modelViewMat);
        objShader.setUniformVec3("lightColor", lightColor);
        objShader.setUniformVec3("lightPos", viewLightPos);
        cube.draw();

        // Create light matrix and reuse camera transformation
        modelMat = glm::mat4(1.0f);
        modelMat = glm::scale(modelMat, glm::vec3(0.2f));
        modelMat = glm::translate(modelMat, lightPos);
        modelViewMat = viewMat * modelMat;
        transformMat = projectionMat * modelViewMat;

        // Set light shader uniforms and draw light
        lightShader.use();
        lightShader.setUniformMat4("transformMat", transformMat);
        cube.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

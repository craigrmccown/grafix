#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "circular_write_buffer.hpp"
#include "clock.hpp"
#include "controls.hpp"
#include "image.hpp"
#include "model.hpp"
#include "mouse.hpp"
#include "point_light.hpp"
#include "shader.hpp"

const int numLights = 4;

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

struct Position {
    glm::vec3 worldCoords;
    glm::vec3 rotationAxis;
    float rotationDeg;
};

Position objPositions[] = {
    {
        .worldCoords = glm::vec3(0.0f, 0.0f, 0.0f),
        .rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f),
        .rotationDeg = 45.0f,
    },
    {
        .worldCoords = glm::vec3(8.0f, 3.0f, 8.0f),
        .rotationAxis = glm::vec3(0.15f, -0.6f, 0.5f),
        .rotationDeg = 60.0f,
    },
    {
        .worldCoords = glm::vec3(1.0f, 8.0f, 7.0f),
        .rotationAxis = glm::vec3(0.75f, 0.9f, 0.75f),
        .rotationDeg = 15.0f,
    },
    {
        .worldCoords = glm::vec3(6.0f, -6.0f, -1.0f),
        .rotationAxis = glm::vec3(-0.85f, 0.5f, -0.3f),
        .rotationDeg = 0.0f,
    },
    {
        .worldCoords = glm::vec3(-3.0f, -8.0f, -6.0f),
        .rotationAxis = glm::vec3(0.65f, 0.15f, -1.0f),
        .rotationDeg = 30.0f,
    },
    {
        .worldCoords = glm::vec3(-7.0f, -1.0f, -3.0f),
        .rotationAxis = glm::vec3(0.2f, -0.25f, 0.1f),
        .rotationDeg = 85.0f,
    },
    {
        .worldCoords = glm::vec3(-5.0f, 5.0f, 4.0f),
        .rotationAxis = glm::vec3(0.7f, -0.45f, 0.25f),
        .rotationDeg = 75.0f,
    },
    {
        .worldCoords = glm::vec3(4.0f, -2.0f, 6.0f),
        .rotationAxis = glm::vec3(0.1f, 0.2f, -0.9f),
        .rotationDeg = 25.0f,
    },
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

int main()
{
    srand(time(nullptr)); // Initialize RNG
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
    mouse::listenForMovement(window);

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
    KeyboardMouseControls ctrl(*window, 0.2f);
    Camera camera(ctrl, glm::vec3(0.0f, 0.0f, 10.0f));
    glm::vec3 globalLightColor(1.0f, 1.0f, 1.0f);
    glm::vec4 globalLightDir(0.0f, -1.0f, 0.0f, 0.0f);

    // Instantiate light buffer - when one is added, the oldest one will
    // disappear.
    CircularWriteBuffer<PointLight> pointLights(numLights);
    pointLights.fill(PointLight().hide());

    // Initialize projection matrix outside of render loop. Use an arbitrary
    // 45 degree field of view
    glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window))
    {
        clock.tick();
        ctrl.processInput(clock.getElapsedSeconds());
        camera.processInput();

        if (ctrl.queryBinaryAction(Controls::BinaryAction::exit, Controls::BinaryActionState::on))
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (ctrl.queryBinaryAction(Controls::BinaryAction::action1, Controls::BinaryActionState::leading))
        {
            pointLights.write(PointLight().randColor().setPosition(camera.getPosition() + camera.getDirection()));
        }

        glm::mat4 viewMat = camera.getViewMatrix();

        // Fill background color first
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw lights first. Cache the view coordinates to avoid computing
        // twice.
        glm::vec3 lightViewCoords[pointLights.getSize()];
        lightShader.use();

        for (int i = 0; i < pointLights.getSize(); i ++)
        {
            PointLight light = pointLights[i];
            if (light.isHidden()) continue;

            glm::vec3 lightPos = pointLights[i].getPosition();
            glm::mat4 lightMat(1.0f);
            lightMat = glm::translate(lightMat, lightPos);
            lightMat = glm::scale(lightMat, glm::vec3(0.2f));
            glm::mat4 lightViewMat = viewMat * lightMat;
            glm::mat4 lightTransformMat = projectionMat * lightViewMat;

            // Transform the light's position to view space for later use - we
            // use view space in the shader for lighting calculations
            lightViewCoords[i] = lightViewMat * glm::vec4(lightPos, 1.0f);

            // Set light shader uniforms and draw light
            lightShader.setUniformMat4("transformMat", lightTransformMat);
            lightShader.setUniformVec3("lightColor", pointLights[i].getColor());
            cube.draw();
        }

        // Now draw the rest of the objects
        objShader.use();
        objShader.setUniformFloat("material.shininess", 64.0f);
        objShader.setUniformVec3("globalLight.color", globalLightColor);
        objShader.setUniformVec3("globalLight.direction", viewMat * globalLightDir);
        objShader.setUniformFloat("globalLight.reflection.ambient", 0.1f);
        objShader.setUniformFloat("globalLight.reflection.diffuse", 0.5f);
        objShader.setUniformFloat("globalLight.reflection.specular", 0.5f);

        // Set light uniforms before processing each object individually
        for (int i = 0; i < pointLights.getSize(); i ++)
        {
            objShader.setUniformVec3Element("pointLights", "color", i, pointLights[i].getColor());
            objShader.setUniformVec3Element("pointLights", "position", i, lightViewCoords[i]);
            objShader.setUniformFloatElement("pointLights", "reflection.ambient", i, 0.05f);
            objShader.setUniformFloatElement("pointLights", "reflection.diffuse", i, 0.8f);
            objShader.setUniformFloatElement("pointLights", "reflection.specular", i, 1.2f);
            objShader.setUniformFloatElement("pointLights", "constant", i, 1.0f);
            objShader.setUniformFloatElement("pointLights", "linear", i, 0.09f);
            objShader.setUniformFloatElement("pointLights", "quadratic", i, 0.032f);
        }

        for (int i = 0; i < sizeof(objPositions) / sizeof(Position); i ++)
        {
            // Create model matrix and transform to camera perspective
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), objPositions[i].worldCoords);
            modelMat = glm::rotate(modelMat, glm::radians(objPositions[i].rotationDeg), objPositions[i].rotationAxis);
            glm::mat4 modelViewMat = viewMat * modelMat;
            glm::mat4 modelTransformMat = projectionMat * modelViewMat;

            // Set obj shader uniforms and draw model
            // TODO: Pass normal matrix instead of model matrix to account for
            // non-uniform scaling
            objShader.setUniformMat4("transformMat", modelTransformMat);
            objShader.setUniformMat4("modelViewMat", modelViewMat);
            cube.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

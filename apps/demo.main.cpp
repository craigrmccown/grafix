#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <core/camera.hpp>
#include <core/circular_write_buffer.hpp>
#include <core/clock.hpp>
#include <core/controls.hpp>
#include <core/image.hpp>
#include <core/model.hpp>
#include <core/mouse.hpp>
#include <core/point_light.hpp>
#include <orc/shader.hpp>

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

void initializeDebugUi(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "var/imgui.ini";
    ImGui::GetIO().LogFilename = "var/log/imgui.log";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
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

void buildDebugUi()
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");

    ImGui::Text("This is some useful text.");

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    if (ImGui::Button("Button"))
    {
        counter++;
    }
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}

void cleanupDebugUi()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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
    initializeDebugUi(window);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return kill("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);

    mouse::listenForMovement(window);

    std::unique_ptr<orc::OpenGLShader> objShader = orc::LoadShaderFromFiles(
        std::vector<std::string> {
            "../../libs/orc/src/orc/shaders/regular.vert",
            "../../libs/orc/src/orc/shaders/regular.frag",
        }
    );
    std::unique_ptr<orc::OpenGLShader> lightShader = orc::LoadShaderFromFiles(
        std::vector<std::string> {
            "../../libs/orc/src/orc/shaders/light.vert",
            "../../libs/orc/src/orc/shaders/light.frag",
        }
    );

    Model cube(
        std::vector<float>(std::begin(vertices), std::end(vertices)),
        "../../data/textures/crate.png"
    );
    cube.load();

    Clock clock;
    KeyboardMouseControls ctrl(*window, 0.2f);
    Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
    glm::vec3 globalLightColor(1.0f, 1.0f, 1.0f);
    glm::vec4 globalLightDir(0.0f, -1.0f, 0.0f, 0.0f);
    bool isFlashlightOn = false;

    // Instantiate light buffer - when one is added, the oldest one will
    // disappear.
    CircularWriteBuffer<PointLight> pointLights(numLights);
    pointLights.fill(PointLight().hide());

    // Initialize projection matrix outside of render loop. Use an arbitrary
    // 45 degree field of view
    glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window))
    {
        // Notify ImGui of each frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        clock.tick();
        ctrl.newFrame(clock.getElapsedSeconds());
        camera.move(
            glm::vec2(ctrl.getValue(Controls::Signal::moveX), ctrl.getValue(Controls::Signal::moveY)),
            glm::vec2(ctrl.getValue(Controls::Signal::aimX), ctrl.getValue(Controls::Signal::aimY))
        );

        if (ctrl.isLeading(Controls::Signal::exit)) glfwSetWindowShouldClose(window, true);
        if (ctrl.isLeading(Controls::Signal::action1)) pointLights.write(PointLight()
            .randColor()
            .setPosition(camera.getPosition() + camera.getDirection()
        ));
        if (ctrl.isLeading(Controls::Signal::action2)) isFlashlightOn = !isFlashlightOn;

        glm::mat4 viewMat = camera.getViewMatrix();

        // Fill background color first
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw lights first. Cache the view coordinates to avoid computing
        // twice.
        glm::vec3 lightViewCoords[pointLights.getSize()];
        lightShader->Use();

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
            lightShader->SetUniformMat4("transformMat", lightTransformMat);
            lightShader->SetUniformVec3("lightColor", pointLights[i].getColor());
            cube.draw();
        }

        // Now draw the rest of the objects
        objShader->Use();
        objShader->SetUniformFloat("material.shininess", 64.0f);
        objShader->SetUniformVec3("globalLight.color", globalLightColor);
        objShader->SetUniformVec3("globalLight.direction", viewMat * globalLightDir);
        objShader->SetUniformFloat("globalLight.reflection.ambient", 0.1f);
        objShader->SetUniformFloat("globalLight.reflection.diffuse", 0.5f);
        objShader->SetUniformFloat("globalLight.reflection.specular", 0.5f);

        // Set light uniforms before processing each object individually
        for (int i = 0; i < pointLights.getSize(); i ++)
        {
            objShader->SetUniformVec3Element("pointLights", "color", i, pointLights[i].getColor());
            objShader->SetUniformVec3Element("pointLights", "position", i, lightViewCoords[i]);
            objShader->SetUniformFloatElement("pointLights", "reflection.ambient", i, 0.05f);
            objShader->SetUniformFloatElement("pointLights", "reflection.diffuse", i, 0.8f);
            objShader->SetUniformFloatElement("pointLights", "reflection.specular", i, 1.2f);
            objShader->SetUniformFloatElement("pointLights", "constant", i, 1.0f);
            objShader->SetUniformFloatElement("pointLights", "linear", i, 0.09f);
            objShader->SetUniformFloatElement("pointLights", "quadratic", i, 0.032f);
        }

        objShader->SetUniformVec3("spotLight.color", glm::vec3(isFlashlightOn ? 1.0f : 0.0f));
        objShader->SetUniformVec3("spotLight.direction", glm::vec3(0.0f, 0.0f, -1.0f));
        objShader->SetUniformVec3("spotLight.position", glm::vec3(0.0f));
        objShader->SetUniformFloat("spotLight.inner", cos(glm::radians(10.0f)));
        objShader->SetUniformFloat("spotLight.outer", cos(glm::radians(15.0f)));
        objShader->SetUniformFloat("spotLight.reflection.ambient", 0.1f);
        objShader->SetUniformFloat("spotLight.reflection.diffuse", 0.5f);
        objShader->SetUniformFloat("spotLight.reflection.specular", 0.5f);

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
            objShader->SetUniformMat4("transformMat", modelTransformMat);
            objShader->SetUniformMat4("modelViewMat", modelViewMat);
            cube.draw();
        }

        // Render the debug UI
        buildDebugUi();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanupDebugUi();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

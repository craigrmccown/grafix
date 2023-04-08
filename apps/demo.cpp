#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <core/clock.hpp>
#include <core/controls.hpp>
#include <core/mouse.hpp>
#include <orc/cube.hpp>
#include <orc/light.hpp>
#include <orc/model.hpp>
#include <orc/object.hpp>
#include <orc/scene.hpp>
#include <orc/shader.hpp>

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

void buildDebugUi(const orc::Scene &scene)
{
    glm::vec3 cameraPos = scene.GetCamera().GetPosition();

    ImGui::Begin("Debug Window");

    ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

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

    // TODO: Engine-supported API for initialization of graphics/rendering APIs
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_FRAMEBUFFER_SRGB); // Enable hardware-implemented gamma correction

    mouse::listenForMovement(window);

    orc::Scene scene;
    scene.GetCamera().SetAspectRatio((float)windowWidth/(float)windowHeight);
    scene.GetCamera().Translate(17.5, 1, -1);
    scene.GetCamera().Rotate(glm::radians(90.0f), 0, 0);

    // This model is not checked into source control for practical reasons
    std::shared_ptr<orc::Object> object = orc::LoadModel("data/models/sponza/scene.gltf");
    scene.GetRoot().AttachChild(object);

    std::shared_ptr<orc::Object> object2 = orc::LoadModel("data/models/legion_commander/scene.gltf");
    object2->Scale(0.01, 0.01, 0.01);
    object2->Translate(0, 0.05, 0);
    object2->Rotate(glm::radians(90.0f), 0, 0);
    scene.GetRoot().AttachChild(object2);

    std::shared_ptr<orc::SpotLight> flash = orc::SpotLight::Create();

    Clock clock;
    KeyboardMouseControls ctrl(*window, 0.2f);
    bool isFlashlightOn = false;

    while(!glfwWindowShouldClose(window))
    {
        // Notify ImGui of each frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        clock.tick();
        ctrl.newFrame(clock.getElapsedSeconds());

        // TODO: Build orientation-specific translation into Node API (e.g. TranslateFront())
        glm::vec3 cameraTranslation = 6.0f * (
            scene.GetCamera().GetFront() * (float)ctrl.getValue(Controls::Signal::moveY) +
            scene.GetCamera().GetRight() * (float)ctrl.getValue(Controls::Signal::moveX)
        );
        glm::vec3 cameraRotation(
            -glm::radians(ctrl.getValue(Controls::Signal::aimX)),
            glm::radians(ctrl.getValue(Controls::Signal::aimY)),
            0
        );
        scene.GetCamera().Translate(cameraTranslation.x, cameraTranslation.y, cameraTranslation.z);
        scene.GetCamera().Rotate(cameraRotation.x, cameraRotation.y, cameraRotation.z);

        if (ctrl.isLeading(Controls::Signal::exit)) glfwSetWindowShouldClose(window, true);
        if (ctrl.isLeading(Controls::Signal::action2))
        {
            isFlashlightOn = !isFlashlightOn;
            if (isFlashlightOn) scene.GetCamera().AttachChild(flash);
            else flash->Detach();
        }

        // Fill background color first
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.Update();
        scene.Draw();

        // Render the debug UI
        buildDebugUi(scene);
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

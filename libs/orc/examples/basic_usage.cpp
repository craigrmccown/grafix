#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "orc/cube.hpp"
#include "orc/light.hpp"
#include "orc/object.hpp"
#include "orc/scene.hpp"

// GLFW window boilerplate to ensure cleanup. Must be used as a singleton.
class ExampleWindow
{
    public:
    ExampleWindow(std::string title, int width, int height)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == nullptr)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLFW context");
        }

        glfwMakeContextCurrent(window);
    }

    ~ExampleWindow()
    {
        glfwTerminate();
    }

    void StartRenderLoop(std::function<void()> render)
    {
        while (!glfwWindowShouldClose(window))
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }

            render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    private:
    GLFWwindow *window;
};

void loadOpenGL()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

int main(int argc, char *argv[])
{
    // Initialize window and OpenGL
    float windowWidth = 800, windowHeight = 600;
    ExampleWindow window("basic_usage", windowWidth, windowHeight);
    loadOpenGL();
    glEnable(GL_DEPTH_TEST);

    // Create scene and position camera
    orc::Scene scene;
    scene.GetCamera().SetAspectRatio(windowWidth/windowHeight);
    scene.GetCamera().Translate(6, 6, 12);
    scene.GetCamera().Rotate(M_PI_4/1.25, -M_PI_4/2.5, 0);

    // Add some lighting
    std::shared_ptr<orc::OmniLight> light = orc::OmniLight::Create();
    light->SetColor(1, 0, 0);
    light->Translate(-8, 6, 2);
    light->Scale(0.5, 0.5, 0.5)
    light->AddMesh(orc::BuildCubeMesh("data"));
    scene.GetRoot().AttachChild(light);

    // Create object and add to scene root
    std::shared_ptr<orc::Object> parent = orc::Object::Create();
    parent->AddMesh(orc::BuildCubeMesh("data"));
    scene.GetRoot().AttachChild(parent);

    // Create child object and attach to parent
    std::shared_ptr<orc::Object> child = orc::Object::Create();
    child->Translate(4, 0, 0);
    child->AddMesh(orc::BuildCubeMesh("data"));
    parent->AttachChild(child);

    // Each frame, nodes should be manipulated as needed, then Update should be
    // called once. At the end of the frame, call Draw.
    window.StartRenderLoop([&scene, &parent, &child]{
        float t = glfwGetTime();
        parent->SetRotation(0, 0, t);
        child->SetRotation(0, t, 0);

        scene.Update();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.Draw();
    });

    return 0;
}

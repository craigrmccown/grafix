#define CATCH_CONFIG_RUNNER
#include <iostream>
#include <catch2/catch_session.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "test", NULL, NULL);
    glfwMakeContextCurrent(window);

    std::string err = "";
    if (window == nullptr)
    {
        err = "Failed to initialize GLFW context";
    }
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        err = "Failed to initialize GLAD";
    }
    if (err != "")
    {
        std::cerr << err << std::endl;
        glfwTerminate();
        return -1;
    }

    int result = Catch::Session().run(argc, argv);
    glfwTerminate();

    return result;
}

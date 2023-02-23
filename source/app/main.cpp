#include "core/solis_engine.hpp"
#include "GLFW/glfw3.h"

using namespace solis;

GLFWwindow *window;

void InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, "Solis", nullptr, nullptr);
}

void CleanupWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    InitWindow();

    EngineCreateInfo info;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++)
    {
        info.extensions.push_back(glfwExtensions[i]);
    }

    Engine engine(info);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    engine.Destroy();
    CleanupWindow();
    return 0;
}
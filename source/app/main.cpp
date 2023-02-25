#include "core/solis_engine.hpp"
#include "core/base/using.hpp"

#include "GLFW/glfw3.h"

#ifdef __WIN__
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __LINUX__
#define GLFW_EXPOSE_NATIVE_X11
#elif __ANDROID__
#define GLFW_EXPOSE_NATIVE_ANDROID
#elif __MAC__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

using namespace solis;

GLFWwindow *window;
math::uvec2 windowSize{800, 600};

void InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(windowSize.x, windowSize.y, "Solis", nullptr, nullptr);
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

    info.window = glfwGetWin32Window(window);
    info.windowSize = windowSize;

    Engine engine(info);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    engine.Destroy();
    CleanupWindow();
    return 0;
}
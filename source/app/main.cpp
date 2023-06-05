#include "core/solis_engine.hpp"
#include "core/base/using.hpp"
#include "core/base/memory.hpp"
#include "core/base/ecs.hpp"

#include "core/log/log.hpp"
#include "core/files/files.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/swapchain.hpp"
#include "core/graphics/pipeline/pipeline_graphics.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/command/command_buffer.hpp"

#include "core/data/model.hpp"
#include "core/data/texture.hpp"

#include "GLFW/glfw3.h"
#include <xcb/xproto.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
#include <iostream>

#if defined(_DEBUG)
#pragma pop_macro("getcwd")
#endif

#include "folly/GLog.h"
#include "folly/AtomicLinkedList.h"

#include "main_world.hpp"

using namespace solis;

GLFWwindow *window;
math::ivec2 windowSize{1377, 768};

float      CameraSpeed{1.0f};
math::vec3 CameraPos{2.0f, 2.0f, 2.0f};

#ifdef __WIN__
_CrtMemState s1, s2, s3;
#endif

void ProcessInput(GLFWwindow *window)
{
    // wasd
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        CameraPos.z -= 0.1f * CameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        CameraPos.z += 0.1f * CameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        CameraPos.x -= 0.1f * CameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        CameraPos.x += 0.1f * CameraSpeed;
    }
}

void InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(windowSize.x, windowSize.y, "Solis", nullptr, nullptr);
    // glfwSetKeyCallback(window, KeyCallback);
}

void CleanupWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char **argv)
{
#if defined(__WINW__)
    _CrtMemCheckpoint(&s1);
#endif

    // google::InitGoogleLogging(argv[0]);
    // FLAGS_logtostderr      = true;
    // FLAGS_colorlogtostderr = true;

    InitWindow();

    EngineCreateInfo info;

    uint32_t     glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++)
    {
        info.extensions.push_back(glfwExtensions[i]);
    }

    // info.extensions.push_back("VK_KHR_get_physical_device_properties2");
    // info.extensions.push_back("VK_EXT_device_fault");

#if defined(__WIN__)
    info.window = glfwGetWin32Window(window);
#elif defined(__LINUX__)

#if defined(USE_XCB) || defined(USE_XLIB)
    info.window        = (void *)glfwGetX11Window(window);
    info.createSurface = [=](VkInstance instance, VkSurfaceKHR &surface) {
        glfwCreateWindowSurface(instance, window, nullptr, &surface);
    };
#elif defined(USE_WAYLAND)
    info.window = glfwGetWaylandWindow(window);
#elif defined(USE_DIRECTFB)
    info.window = glfwGetDirectFBWindow(window);
#endif

#elif defined(__ANDROID__)
    info.window = glfwGetAndroidWindow(window);
#elif defined(__MAC__)
    info.window = glfwGetCocoaWindow(window);
#endif
    info.windowSize = windowSize;

    // info.renderGraph = "default.graph"
    Engine engine(info);
    engine.SetMainWorld(std::make_unique<MainWorld>());

    // using namespace graphics;
    // using namespace files;

    // RenderPass renderPass;
    // renderPass.Build();

    // PipelineGraphics pipeline;
    // auto            &shader = pipeline.GetShader();
    // // shader.CreateShaderModule("./shaders/triangle/triangle.vert", Shader::Type::Vertex);
    // // shader.CreateShaderModule("./shaders/triangle/triangle.frag", Shader::Type::Fragment);
    // shader.CreateShaderModule("./shaders/sponza/sponza.vert", Shader::Type::Vertex);
    // shader.CreateShaderModule("./shaders/sponza/sponza.frag", Shader::Type::Fragment);
    // pipeline.Build(renderPass);
    {
        // Model   sponzamodel{"./gltfs/cube/cube.gltf"};
        // Texture texture("./gltfs/cube/swap.jpg");
        // // pipeline.BindUniform(0);
        // // pipeline.BindTexture(texture, 1);
        // // pipeline.UpdateBinding();
        // pipeline.InitUniformBuffers(sizeof(UniformBufferObject));
        // pipeline.BindEverything(texture, 1);

        // CommandBuffer buffer;
        // Swapchain    &swapchain = engine.GetSwapchain();
        // swapchain.SetRenderPass(renderPass);

        while (!glfwWindowShouldClose(window))
        {
            // FB_LOG_EVERY_MS(WARNING, 1000) << "Test";
            glfwGetFramebufferSize(window, &windowSize.x, &windowSize.y);
            while (windowSize.x == 0 || windowSize.y == 0)
            {
                glfwGetFramebufferSize(window, &windowSize.x, &windowSize.y);
                glfwWaitEvents();
            }
            glfwPollEvents();
            ProcessInput(window);
            engine.Step();
            // if (!swapchain.IsSameExtent({(uint32_t)windowSize.x, (uint32_t)windowSize.y}))
            // {
            // swapchain.Recreate({(uint32_t)windowSize.x, (uint32_t)windowSize.y});
            // }

            // if (swapchain.AcquireNextImage() != VK_SUCCESS)
            // {
            // continue;
            // }

            // // CommandBuffer &buffer = swapchain.GetCommandBuffer();
            // buffer.Wait();
            // buffer.Begin();
            // buffer.BeginRenderPass(swapchain);
            // buffer.BindPipeline(&pipeline);

            // // ubo
            // // buffer->BindUniformBuffer(0, 0, pipeline->GetUniformBuffer(swapchain.GetImageIndex()));
            // auto &ubo = pipeline.GetUniformBuffer(swapchain.GetActiveImageIndex());

            // static auto startTime   = std::chrono::high_resolution_clock::now();
            // auto        currentTime = std::chrono::high_resolution_clock::now();
            // float       time        = st::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            // auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // auto view  = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // auto proj  = glm::perspective(glm::radians(90.0f), windowSize.x / (float)windowSize.y, 0.1f, 10000.0f);
            // proj[1][1] *= -1;

            // auto mvp = proj * view * model;
            // // auto mvp = proj * view;
            // ubo.Update(&mvp, sizeof(mvp));
            // // sampler

            // buffer.SetViewport({0, 0, (float)windowSize.x, (float)windowSize.y, 0, 1});
            // buffer.SetScissor({0, 0, (unsigned int)windowSize.x, (unsigned int)windowSize.y});
            // // buffer->Draw(3, 1, 0, 0);
            // buffer.Draw(sponzamodel);
            // buffer.EndRenderPass();
            // buffer.End();
            // swapchain.SubmitCommandBuffer(buffer);
        }
    }

    // 这儿还有问题， 不过问题不大
    engine.Destroy();
    CleanupWindow();
    if (ObjectBase::ObjectCount != 0)
    {
        for (auto &[pointer, name] : ObjectBase::ObjectTypeNameMap)
        {
            Log::SError("Object Class Name: {} is leaked! pointer: {}", name, pointer);
        }
    }
    ObjectBase::Clear();

    // google::ShutdownGoogleLogging();

#if defined(__WIN__) && defined(__DEBUG__)
    // 这里的内存泄露是查的最准的，因为这里没有算加载glog和gflags的导致增加的内存
    // 这里会因为folly的hash_map导致有1k的内存无法手动释放(静态代码段里malloc内存，除非手动new和delete)
    _CrtMemCheckpoint(&s2);
    if (_CrtMemDifference(&s3, &s1, &s2))
    {
        _CrtMemDumpStatistics(&s3);
    }
#endif
    return 0;
}

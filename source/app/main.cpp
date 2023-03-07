#include "core/solis_engine.hpp"
#include "core/base/using.hpp"

#include "core/log/log.hpp"

#include "core/files/files.hpp"
#include "core/graphics/graphics.hpp"
#include "core/graphics/pipeline/shader.hpp"

#include "core/graphics/swapchain.hpp"
#include "core/graphics/pipeline/pipeline_graphics.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/command/command_buffer.hpp"

#include "core/data/model.hpp"

#include "GLFW/glfw3.h"

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

using namespace solis;

GLFWwindow *window;
math::ivec2 windowSize{1377, 768};

void InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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

    uint32_t     glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++)
    {
        info.extensions.push_back(glfwExtensions[i]);
    }

    info.window     = glfwGetWin32Window(window);
    info.windowSize = windowSize;

    Engine engine(info);

    using namespace graphics;
    using namespace files;

    Files::Get()->SetCurrentPath(Files::Get()->GetExecuteBinPath());

    // Shader shader;
    // shader.CreateShaderModule("./shaders/triangle/triangle.vert", Shader::Type::Vertex);
    // shader.CreateShaderModule("./shaders/triangle/triangle.frag", Shader::Type::Fragment);

    RenderPass *renderPass = new RenderPass();
    renderPass->Build();

    PipelineGraphics *pipeline = new PipelineGraphics();
    auto             &shader   = pipeline->GetShader();
    // shader.CreateShaderModule("./shaders/triangle/triangle.vert", Shader::Type::Vertex);
    // shader.CreateShaderModule("./shaders/triangle/triangle.frag", Shader::Type::Fragment);
    shader.CreateShaderModule("./shaders/sponza/sponza.vert", Shader::Type::Vertex);
    shader.CreateShaderModule("./shaders/sponza/sponza.frag", Shader::Type::Fragment);
    pipeline->Build(*renderPass);

    Model sponzamodel{"./gltfs/sponza/Sponza.gltf"};

    CommandBuffer *buffer    = new CommandBuffer();
    Swapchain     &swapchain = engine.GetSwapchain();
    swapchain.SetRenderPass(*renderPass);

    pipeline->ResizeUniformBuffers(swapchain.GetImageCount());
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &windowSize.x, &windowSize.y);
        while (windowSize.x == 0 || windowSize.y == 0)
        {
            glfwGetFramebufferSize(window, &windowSize.x, &windowSize.y);
            glfwWaitEvents();
        }
        glfwPollEvents();

        if (!swapchain.IsSameExtent({(uint32_t)windowSize.x, (uint32_t)windowSize.y}))
        {
            swapchain.Recreate({(uint32_t)windowSize.x, (uint32_t)windowSize.y});
        }

        if (swapchain.AcquireNextImage() != VK_SUCCESS)
        {
            continue;
        }

        buffer->Begin();
        buffer->BeginRenderPass(swapchain);
        buffer->BindPipeline(pipeline);

        // ubo
        // buffer->BindUniformBuffer(0, 0, pipeline->GetUniformBuffer(swapchain.GetImageIndex()));
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto  currentTime = std::chrono::high_resolution_clock::now();
        float time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto &ubo   = pipeline->GetUniformBuffer(&swapchain, swapchain.GetActiveImageIndex());
        auto  model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto  view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto  proj  = glm::perspective(glm::radians(45.0f), windowSize.x / (float)windowSize.y, 0.1f, 10.0f);
        proj[1][1] *= -1;

        auto mvp = proj * view * model;
        ubo.Update(&mvp, sizeof(mvp));

        buffer->SetViewport({0, 0, (float)windowSize.x, (float)windowSize.y, 0, 1});
        buffer->SetScissor({0, 0, (unsigned int)windowSize.x, (unsigned int)windowSize.y});
        // buffer->Draw(3, 1, 0, 0);
        buffer->Draw(sponzamodel);
        buffer->EndRenderPass();
        buffer->End();
        swapchain.SubmitCommandBuffer(*buffer);
    }

    // 这儿还有问题， 不过问题不大
    delete buffer;
    delete pipeline;
    delete renderPass;

    engine.Destroy();
    CleanupWindow();
    return 0;
}
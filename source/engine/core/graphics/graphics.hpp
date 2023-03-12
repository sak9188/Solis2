#pragma once

#include <memory>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

#include "volk.h"

namespace solis {
namespace graphics {
class Instance;
class PhysicalDevice;
class LogicalDevice;
class Surface;
class Swapchain;
class CommandPool;
class RenderPass;
class Pipeline;
class Buffer;

struct UniformBufferObject
{
    math::mat4 mvp;
};

class SOLIS_CORE_API Graphics : public Object<Graphics>, public Module::Registrar<Graphics>
{
    inline static const bool Registered = Register(Stage::Render);

public:
    Graphics();
    virtual ~Graphics();

    void Init();

    virtual void Update() override
    {
    }

    // GetLogicalDevice()
    LogicalDevice *GetLogicalDevice() const
    {
        return mLogicalDevice.get();
    }

    // GetPhysicalDevice()
    PhysicalDevice *GetPhysicalDevice() const
    {
        return mPhysicalDevice.get();
    }

    const Instance *GetInstance() const
    {
        return mInstance.get();
    }

    std::shared_ptr<CommandPool> GetCommandPool() const
    {
        return mCommandPool;
    }

    // AddRenderPass
    void AddRenderPass(RenderPass *renderPass) noexcept
    {
        mRenderPasses.push_back(renderPass);
    }

    // AddPipeline
    void AddPipeline(Pipeline *pipeline) noexcept
    {
        mPipelines.push_back(pipeline);
    }

    // AddBuffer
    // void AddBuffer(Buffer *buffer) noexcept
    // {
    //     mBuffers.push_back(buffer);
    // }

    Swapchain *CreateSurfaceSwapchain(const void *window, math::uvec2 extent);

    Swapchain *CreateSurfaceSwapchain(const void *window, VkExtent2D extent);

    static string StringifyResultVk(VkResult result);

    static void CheckVk(VkResult result);

private:
    std::unique_ptr<Instance>       mInstance;
    std::unique_ptr<PhysicalDevice> mPhysicalDevice;
    std::unique_ptr<LogicalDevice>  mLogicalDevice;
    std::shared_ptr<CommandPool>    mCommandPool;

    vector<std::unique_ptr<Surface>>   mSurfaces;
    vector<std::unique_ptr<Swapchain>> mSwapchains;

    vector<RenderPass *> mRenderPasses;
    vector<Pipeline *>   mPipelines;
    // vector<Buffer *>     mBuffers;
};
}
} // namespace solis::graphics

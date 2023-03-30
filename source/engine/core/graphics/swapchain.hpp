#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/graphics/render_pass.hpp"

#include "volk.h"
namespace solis {
namespace graphics {
class PhysicalDevice;
class Surface;
class LogicalDevice;
class CommandBuffer;

class SOLIS_CORE_API Swapchain : public Object<Swapchain>, public IDestroyable
{
    friend class Graphics;

public:
    Swapchain(const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const Surface &surface, const VkExtent2D &extent, const Swapchain *oldSwapchain = nullptr);
    ~Swapchain();

    virtual void Destroy() override;

    // 重建Swapchain
    void Recreate(const VkExtent2D &extent);

    void Recreate()
    {
        Recreate(this->mExtent);
    }

    // 设置渲染通道
    void SetRenderPass(RenderPass &renderPass);

    // 渲染相关
    VkResult AcquireNextImage();
    VkResult QueuePresent();

    bool IsSameExtent(const VkExtent2D &extent2D)
    {
        return mExtent.width == extent2D.width && mExtent.height == extent2D.height;
    }

    operator const VkSwapchainKHR &() const
    {
        return mSwapchain;
    }

    const VkExtent2D &GetExtent() const
    {
        return mExtent;
    }

    uint32_t GetFramesCount() const
    {
        return mFrames;
    }

    VkSurfaceTransformFlagsKHR GetPreTransform() const
    {
        return preTransform;
    }
    VkCompositeAlphaFlagBitsKHR GetCompositeAlpha() const
    {
        return compositeAlpha;
    }

    RenderPass *GetRenderPass() const
    {
        return mRenderPass;
    }
    VkFramebuffer GetActiveFrameBuffer() const
    {
        return mFrameBuffers[GetActiveImageIndex()];
    }
    VkFence GetWaitFence() const
    {
        return mWaitFences[GetActiveImageIndex()];
    }
    VkSemaphore GetImageAvailableSemaphore() const
    {
        return mImageAvailableSemaphores[GetActiveImageIndex()];
    }
    VkSemaphore GetRenderFinishedSemaphore() const
    {
        return mRenderFinishedSemaphores[GetActiveImageIndex()];
    }

    const vector<VkImage> &GetImages() const
    {
        return images;
    }
    const VkImage &GetActiveImage() const
    {
        return images[GetActiveImageIndex()];
    }
    const vector<VkImageView> &GetImageViews() const
    {
        return imageViews;
    }
    const VkSwapchainKHR &GetSwapchain() const
    {
        return mSwapchain;
    }
    uint32_t GetActiveImageIndex() const
    {
        return mFrames % mImageCount;
    }

    void SubmitCommandBuffer(CommandBuffer &commandBuffer);

private:
    void Create(const Swapchain *oldSwapchain = nullptr);
    void Clean();

private:
    const PhysicalDevice &mPhysicalDevice;

    const Surface       &mSurface;
    const LogicalDevice &mLogicalDevice;

    VkExtent2D       mExtent;
    VkPresentModeKHR mPresentMode;

    VkSurfaceTransformFlagsKHR  preTransform;
    VkCompositeAlphaFlagBitsKHR compositeAlpha;

    // TODO: RenderPass
    RenderPass *mRenderPass = nullptr;

    uint32_t mFrames     = 0;
    uint32_t mImageCount = 0;

    vector<VkImage>       images;
    vector<VkImageView>   imageViews;
    vector<VkFramebuffer> mFrameBuffers;

    vector<VkFence>     mWaitFences;
    vector<VkSemaphore> mImageAvailableSemaphores;
    vector<VkSemaphore> mRenderFinishedSemaphores;

    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
};
}
} // namespace solis::graphics

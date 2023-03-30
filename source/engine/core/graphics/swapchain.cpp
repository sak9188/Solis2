#include "core/graphics/swapchain.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"
#include "core/graphics/surface.hpp"
#include "core/graphics/image/image.hpp"
#include "core/graphics/command/command_buffer.hpp"

#include "core/log/log.hpp"

namespace solis {
namespace graphics {
static const vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
};

Swapchain::Swapchain(const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const Surface &surface, const VkExtent2D &extent, const Swapchain *oldSwapchain) :
    mPhysicalDevice(physicalDevice),
    mSurface(surface),
    mLogicalDevice(logicalDevice),
    mExtent(extent),
    mPresentMode(VK_PRESENT_MODE_FIFO_KHR),
    preTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
    compositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)

{
    this->Create(oldSwapchain);

    mWaitFences.resize(mImageCount);
    mImageAvailableSemaphores.resize(mImageCount);
    mRenderFinishedSemaphores.resize(mImageCount);

    for (uint32_t i = 0; i < mImageCount; i++)
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &mWaitFences[i]);
        // Graphics::CheckVk(vkResetFences(logicalDevice, 1, &mWaitFences[i]));

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]);
        vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[i]);
    }
}

Swapchain::~Swapchain()
{
    this->Destroy();
}

void Swapchain::Destroy()
{
    if (IsDestroyed())
    {
        return;
    }

    this->Clean();

    for (const auto &fence : mWaitFences)
    {
        vkDestroyFence(mLogicalDevice, fence, nullptr);
    }
    mWaitFences.clear();

    for (const auto &semaphore : mImageAvailableSemaphores)
    {
        vkDestroySemaphore(mLogicalDevice, semaphore, nullptr);
    }
    mImageAvailableSemaphores.clear();

    for (const auto &semaphore : mRenderFinishedSemaphores)
    {
        vkDestroySemaphore(mLogicalDevice, semaphore, nullptr);
    }
    mRenderFinishedSemaphores.clear();
}

void Swapchain::Clean()
{
    vkDeviceWaitIdle(mLogicalDevice);

    for (auto framebuffer : mFrameBuffers)
    {
        vkDestroyFramebuffer(mLogicalDevice, framebuffer, nullptr);
    }
    mFrameBuffers.clear();

    for (const auto &imageView : imageViews)
    {
        vkDestroyImageView(mLogicalDevice, imageView, nullptr);
    }
    imageViews.clear();

    vkDestroySwapchainKHR(mLogicalDevice, mSwapchain, nullptr);
}

void Swapchain::Create(const Swapchain *oldSwapchain)
{
    auto surfaceFormat       = mSurface.GetFormat();
    auto surfaceCapabilities = mSurface.GetCapabilities();
    auto graphicsFamily      = mLogicalDevice.GetGraphicsFamily();
    auto presentFamily       = mLogicalDevice.GetPresentFamily();

    uint32_t physicalPresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &physicalPresentModeCount, nullptr);
    vector<VkPresentModeKHR> physicalPresentModes(physicalPresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &physicalPresentModeCount, physicalPresentModes.data());

    for (const auto &presentMode : physicalPresentModes)
    {
        // 这个才是垂直同步
        if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
        {
            this->mPresentMode = presentMode;
            break;
        }

        // 这个只是高帧率的防撕裂, 显卡还是会跑满
        // if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        // {
        //     this->presentMode = presentMode;
        //     break;
        // }

        if (presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            this->mPresentMode = presentMode;
        }
    }

    auto desiredImageCount = MaxFrameInFlight;

    if (surfaceCapabilities.maxImageCount > 0 && desiredImageCount > surfaceCapabilities.maxImageCount)
    {
        desiredImageCount = surfaceCapabilities.maxImageCount;
    }

    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        // We prefer a non-rotated transform.
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfaceCapabilities.currentTransform;
    }

    for (const auto &compositeAlphaFlag : COMPOSITE_ALPHA_FLAGS)
    {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
        {
            compositeAlpha = compositeAlphaFlag;
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface                  = mSurface;
    swapchainCreateInfo.minImageCount            = desiredImageCount;
    swapchainCreateInfo.imageFormat              = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace          = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent              = this->mExtent;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform             = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
    swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.compositeAlpha           = compositeAlpha;
    swapchainCreateInfo.presentMode              = mPresentMode;
    swapchainCreateInfo.clipped                  = VK_TRUE;
    swapchainCreateInfo.oldSwapchain             = VK_NULL_HANDLE;

    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if (oldSwapchain)
        swapchainCreateInfo.oldSwapchain = oldSwapchain->mSwapchain;

    if (graphicsFamily != presentFamily)
    {
        std::array<uint32_t, 2> queueFamily       = {graphicsFamily, presentFamily};
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
        swapchainCreateInfo.pQueueFamilyIndices   = queueFamily.data();
    }

    // CreateImage
    Graphics::CheckVk(vkCreateSwapchainKHR(mLogicalDevice, &swapchainCreateInfo, nullptr, &mSwapchain));

    Graphics::CheckVk(vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &mImageCount, nullptr));
    images.resize(mImageCount);
    imageViews.resize(mImageCount);
    Graphics::CheckVk(vkGetSwapchainImagesKHR(mLogicalDevice, mSwapchain, &mImageCount, images.data()));

    for (uint32_t i = 0; i < mImageCount; i++)
    {
        Image::CreateImageView(images.at(i), imageViews.at(i), VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
    }
}

void Swapchain::Recreate(const VkExtent2D &extent)
{
    this->mExtent = extent;
    this->Clean();
    Create(nullptr);

    mFrameBuffers.resize(mImageCount);
    for (uint32_t i = 0; i < mImageCount; i++)
    {
        VkImageView attachments[] = {
            imageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass->GetRenderPass();

        // 下面这两个要从renderpass里面获取
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;

        framebufferInfo.width  = mExtent.width;
        framebufferInfo.height = mExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(*Graphics::Get()->GetLogicalDevice(), &framebufferInfo, nullptr, &mFrameBuffers[i]) != VK_SUCCESS)
        {
            Log::SError("failed to create framebuffer!");
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Swapchain::SetRenderPass(RenderPass &renderPass)
{
    if (mFrameBuffers.size() > 0)
    {
        for (auto &frameBuffer : mFrameBuffers)
        {
            vkDestroyFramebuffer(*Graphics::Get()->GetLogicalDevice(), frameBuffer, nullptr);
        }
        mFrameBuffers.clear();
    }

    mFrameBuffers.resize(mImageCount);
    for (uint32_t i = 0; i < mImageCount; i++)
    {
        VkImageView attachments[] = {
            imageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetRenderPass();

        // 下面这两个要从renderpass里面获取
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;

        framebufferInfo.width  = mExtent.width;
        framebufferInfo.height = mExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(*Graphics::Get()->GetLogicalDevice(), &framebufferInfo, nullptr, &mFrameBuffers[i]) != VK_SUCCESS)
        {
            Log::SError("failed to create framebuffer!");
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    mRenderPass = &renderPass;
}

VkResult Swapchain::AcquireNextImage()
{
    auto activeImageIndex = GetActiveImageIndex();
    auto fence            = mWaitFences[activeImageIndex];
    if (fence != VK_NULL_HANDLE)
    {
        Graphics::CheckVk(vkWaitForFences(mLogicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
    }

    auto imageAvailableSemaphore = mImageAvailableSemaphores[activeImageIndex];
    auto acquireResult           = vkAcquireNextImageKHR(mLogicalDevice, mSwapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &activeImageIndex);

    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR && acquireResult != VK_ERROR_OUT_OF_DATE_KHR)
        throw std::runtime_error("Failed to acquire swapchain image");

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        this->Recreate();
        mFrames += 1;
        return VK_NOT_READY;
    }

    Graphics::CheckVk(vkResetFences(mLogicalDevice, 1, &fence));
    return acquireResult;
}

VkResult Swapchain::QueuePresent()
{
    auto             presentQueue     = mLogicalDevice.GetPresentQueue();
    auto             activeImageIndex = GetActiveImageIndex();
    auto             waitSemaphore    = mRenderFinishedSemaphores[activeImageIndex];
    VkPresentInfoKHR presentInfo      = {};
    presentInfo.sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount    = 1;
    presentInfo.pWaitSemaphores       = &waitSemaphore;
    presentInfo.swapchainCount        = 1;
    presentInfo.pSwapchains           = &mSwapchain;
    presentInfo.pImageIndices         = &activeImageIndex;

    mFrames += 1;
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Swapchain::SubmitCommandBuffer(CommandBuffer &commandbuffer)
{
    commandbuffer.Submit(mImageAvailableSemaphores[GetActiveImageIndex()], mRenderFinishedSemaphores[GetActiveImageIndex()], mWaitFences[GetActiveImageIndex()]);
    QueuePresent();
}
}
} // namespace solis::graphics
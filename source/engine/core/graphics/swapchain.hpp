#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/graphics/render_pass.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class PhysicalDevice;
        class Surface;
        class LogicalDevice;
        class CommandBuffer;

        class SOLIS_CORE_API Swapchain : public Object<Swapchain>
        {
            friend class Graphics;

        public:
            Swapchain(const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const Surface &surface, const VkExtent2D &extent, const Swapchain *oldSwapchain = nullptr);
            ~Swapchain();

            void SetRenderPass(RenderPass &renderPass);

            VkResult AcquireNextImage();
            VkResult QueuePresent();

            bool IsSameExtent(const VkExtent2D &extent2D) { return extent.width == extent2D.width && extent.height == extent2D.height; }

            operator const VkSwapchainKHR &() const { return swapchain; }

            const VkExtent2D &GetExtent() const { return extent; }
            uint32_t GetImageCount() const { return imageCount; }
            VkSurfaceTransformFlagsKHR GetPreTransform() const { return preTransform; }
            VkCompositeAlphaFlagBitsKHR GetCompositeAlpha() const { return compositeAlpha; }

            RenderPass *GetRenderPass() const { return mRenderPass; }
            VkFramebuffer GetActiveFrameBuffer() const { return mFrameBuffers[GetActiveImageIndex()]; }
            VkFence GetWaitFence() const { return mWaitFences[GetActiveImageIndex()]; }
            VkSemaphore GetImageAvailableSemaphore() const { return mImageAvailableSemaphores[GetActiveImageIndex()]; }
            VkSemaphore GetRenderFinishedSemaphore() const { return mRenderFinishedSemaphores[GetActiveImageIndex()]; }

            const vector<VkImage> &GetImages() const { return images; }
            const VkImage &GetActiveImage() const { return images[GetActiveImageIndex()]; }
            const vector<VkImageView> &GetImageViews() const { return imageViews; }
            const VkSwapchainKHR &GetSwapchain() const { return swapchain; }
            uint32_t GetActiveImageIndex() const { return mCurrentFrame % imageCount; }

            void SubmitCommandBuffer(CommandBuffer &commandBuffer);

        private:
            const PhysicalDevice &physicalDevice;
            const Surface &surface;
            const LogicalDevice &logicalDevice;

            VkExtent2D extent;
            VkPresentModeKHR presentMode;

            VkSurfaceTransformFlagsKHR preTransform;
            VkCompositeAlphaFlagBitsKHR compositeAlpha;

            // TODO: RenderPass
            RenderPass *mRenderPass = nullptr;

            uint32_t imageCount = 0;
            size_t mCurrentFrame = 0;

            vector<VkImage> images;
            vector<VkImageView> imageViews;
            vector<VkFramebuffer> mFrameBuffers;

            vector<VkFence> mWaitFences;
            vector<VkSemaphore> mImageAvailableSemaphores;
            vector<VkSemaphore> mRenderFinishedSemaphores;

            VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        };
    }
}

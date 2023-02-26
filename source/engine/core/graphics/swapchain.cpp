#include "core/graphics/swapchain.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"
#include "core/graphics/surface.hpp"
#include "core/graphics/image/image.hpp"
#include "core/graphics/command/command_buffer.hpp"

#include "core/log/log.hpp"

namespace solis
{
    namespace graphics
    {
        static const vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };

        Swapchain::Swapchain(const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const Surface &surface, const VkExtent2D &extent, const Swapchain *oldSwapchain) : physicalDevice(physicalDevice),
                                                                                                                                                                                          surface(surface),
                                                                                                                                                                                          logicalDevice(logicalDevice),
                                                                                                                                                                                          extent(extent),
                                                                                                                                                                                          presentMode(VK_PRESENT_MODE_FIFO_KHR),
                                                                                                                                                                                          preTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR),
                                                                                                                                                                                          compositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)

        {
            auto surfaceFormat = surface.GetFormat();
            auto surfaceCapabilities = surface.GetCapabilities();
            auto graphicsFamily = logicalDevice.GetGraphicsFamily();
            auto presentFamily = logicalDevice.GetPresentFamily();

            uint32_t physicalPresentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, nullptr);
            vector<VkPresentModeKHR> physicalPresentModes(physicalPresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, physicalPresentModes.data());

            for (const auto &presentMode : physicalPresentModes)
            {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    this->presentMode = presentMode;
                    break;
                }

                if (presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    this->presentMode = presentMode;
                }
            }

            auto desiredImageCount = surfaceCapabilities.minImageCount + 1;

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
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.minImageCount = desiredImageCount;
            swapchainCreateInfo.imageFormat = surfaceFormat.format;
            swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
            swapchainCreateInfo.imageExtent = this->extent;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
            swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainCreateInfo.compositeAlpha = compositeAlpha;
            swapchainCreateInfo.presentMode = presentMode;
            swapchainCreateInfo.clipped = VK_TRUE;
            swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

            if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
                swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

            if (oldSwapchain)
                swapchainCreateInfo.oldSwapchain = oldSwapchain->swapchain;

            if (graphicsFamily != presentFamily)
            {
                std::array<uint32_t, 2> queueFamily = {graphicsFamily, presentFamily};
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
                swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
            }

            Graphics::CheckVk(vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain));

            Graphics::CheckVk(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr));
            images.resize(imageCount);
            imageViews.resize(imageCount);
            Graphics::CheckVk(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, images.data()));

            mWaitFences.resize(imageCount);
            mImageAvailableSemaphores.resize(imageCount);
            mRenderFinishedSemaphores.resize(imageCount);

            for (uint32_t i = 0; i < imageCount; i++)
            {
                Image::CreateImageView(images.at(i), imageViews.at(i), VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT,
                                       1, 0, 1, 0);

                VkFenceCreateInfo fenceCreateInfo = {};
                fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &mWaitFences[i]);
                // Graphics::CheckVk(vkResetFences(logicalDevice, 1, &mWaitFences[i]));

                VkSemaphoreCreateInfo semaphoreCreateInfo = {};
                semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]);
                vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[i]);
            }
        }

        Swapchain::~Swapchain()
        {
            vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);

            for (const auto &imageView : imageViews)
            {
                vkDestroyImageView(logicalDevice, imageView, nullptr);
            }

            for (const auto &image : images)
            {
                vkDestroyImage(logicalDevice, image, nullptr);
            }

            for (const auto &fence : mWaitFences)
            {
                vkDestroyFence(logicalDevice, fence, nullptr);
                mWaitFences.clear();
            }

            for (const auto &semaphore : mImageAvailableSemaphores)
            {
                vkDestroySemaphore(logicalDevice, semaphore, nullptr);
                mImageAvailableSemaphores.clear();
            }

            for (const auto &semaphore : mRenderFinishedSemaphores)
            {
                vkDestroySemaphore(logicalDevice, semaphore, nullptr);
                mRenderFinishedSemaphores.clear();
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

            mFrameBuffers.resize(imageCount);
            for (uint32_t i = 0; i < imageCount; i++)
            {
                VkImageView attachments[] = {
                    imageViews[i]};

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass.GetRenderPass();

                // 下面这两个要从renderpass里面获取
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments = attachments;

                framebufferInfo.width = extent.width;
                framebufferInfo.height = extent.height;
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
            auto fence = mWaitFences[activeImageIndex];
            if (fence != VK_NULL_HANDLE)
            {
                Graphics::CheckVk(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
                Graphics::CheckVk(vkResetFences(logicalDevice, 1, &fence));
            }

            auto imageAvailableSemaphore = mImageAvailableSemaphores[activeImageIndex];
            auto acquireResult = vkAcquireNextImageKHR(logicalDevice, swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &activeImageIndex);

            if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR && acquireResult != VK_ERROR_OUT_OF_DATE_KHR)
                throw std::runtime_error("Failed to acquire swapchain image");

            // Graphics::CheckVk(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
            // Graphics::CheckVk(vkResetFences(logicalDevice, 1, &fence));
            return acquireResult;
        }

        VkResult Swapchain::QueuePresent()
        {
            auto presentQueue = logicalDevice.GetPresentQueue();
            auto activeImageIndex = GetActiveImageIndex();
            auto waitSemaphore = mRenderFinishedSemaphores[activeImageIndex];
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &waitSemaphore;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchain;
            presentInfo.pImageIndices = &activeImageIndex;

            imageCount += 1;
            return vkQueuePresentKHR(presentQueue, &presentInfo);
        }

        void Swapchain::SubmitCommandBuffer(CommandBuffer &commandbuffer)
        {
            commandbuffer.Submit(mImageAvailableSemaphores[GetActiveImageIndex()], mRenderFinishedSemaphores[GetActiveImageIndex()], mWaitFences[GetActiveImageIndex()]);
            QueuePresent();
        }
    }
}
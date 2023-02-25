#include "core/graphics/image/image.hpp"
#include "core/graphics/graphics.hpp"
#include "core/graphics/buffer/buffer.hpp"

namespace solis
{
    namespace graphics
    {
        void Image::CreateImage(VkImage &image, VkDeviceMemory &memory, const VkExtent3D &extent, VkFormat format, VkSampleCountFlagBits samples,
                                VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels, uint32_t arrayLayers, VkImageType type)
        {
            auto logicalDevice = Graphics::Get()->GetLogicalDevice();

            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.flags = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
            imageCreateInfo.imageType = type;
            imageCreateInfo.format = format;
            imageCreateInfo.extent = extent;
            imageCreateInfo.mipLevels = mipLevels;
            imageCreateInfo.arrayLayers = arrayLayers;
            imageCreateInfo.samples = samples;
            imageCreateInfo.tiling = tiling;
            imageCreateInfo.usage = usage;
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            Graphics::CheckVk(vkCreateImage(*logicalDevice, &imageCreateInfo, nullptr, &image));

            VkMemoryRequirements memoryRequirements;
            vkGetImageMemoryRequirements(*logicalDevice, image, &memoryRequirements);

            VkMemoryAllocateInfo memoryAllocateInfo = {};
            memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memoryAllocateInfo.allocationSize = memoryRequirements.size;
            memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryType(memoryRequirements.memoryTypeBits, properties);
            Graphics::CheckVk(vkAllocateMemory(*logicalDevice, &memoryAllocateInfo, nullptr, &memory));

            Graphics::CheckVk(vkBindImageMemory(*logicalDevice, image, memory, 0));
        }

        void Image::CreateImageSampler(VkSampler &sampler, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, uint32_t mipLevels)
        {
            auto physicalDevice = Graphics::Get()->GetPhysicalDevice();
            auto logicalDevice = Graphics::Get()->GetLogicalDevice();

            VkSamplerCreateInfo samplerCreateInfo = {};
            samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerCreateInfo.magFilter = filter;
            samplerCreateInfo.minFilter = filter;
            samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerCreateInfo.addressModeU = addressMode;
            samplerCreateInfo.addressModeV = addressMode;
            samplerCreateInfo.addressModeW = addressMode;
            samplerCreateInfo.mipLodBias = 0.0f;
            samplerCreateInfo.anisotropyEnable = static_cast<VkBool32>(anisotropic);
            samplerCreateInfo.maxAnisotropy =
                (anisotropic && logicalDevice->GetEnabledFeatures().samplerAnisotropy) ? std::min(16.0f, physicalDevice->GetProperties().limits.maxSamplerAnisotropy) : 1.0f;
            // samplerCreateInfo.compareEnable = VK_FALSE;
            // samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerCreateInfo.minLod = 0.0f;
            samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
            samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
            Graphics::CheckVk(vkCreateSampler(*logicalDevice, &samplerCreateInfo, nullptr, &sampler));
        }

        void Image::CreateImageView(const VkImage &image, VkImageView &imageView, VkImageViewType type, VkFormat format, VkImageAspectFlags imageAspect,
                                    uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer)
        {
            auto logicalDevice = Graphics::Get()->GetLogicalDevice();

            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = image;
            imageViewCreateInfo.viewType = type;
            imageViewCreateInfo.format = format;
            imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
            imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
            imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
            imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
            imageViewCreateInfo.subresourceRange.layerCount = layerCount;
            Graphics::CheckVk(vkCreateImageView(*logicalDevice, &imageViewCreateInfo, nullptr, &imageView));
        }

        // void Image::CreateMipmaps(const VkImage &image, const VkExtent3D &extent, VkFormat format, VkImageLayout dstImageLayout, uint32_t mipLevels,
        //                           uint32_t baseArrayLayer, uint32_t layerCount)
        // {
        //     auto physicalDevice = Graphics::Get()->GetPhysicalDevice();

        //     // Get device properites for the requested Image format.
        //     VkFormatProperties formatProperties;
        //     vkGetPhysicalDeviceFormatProperties(*physicalDevice, format, &formatProperties);

        //     // Mip-chain generation requires support for blit source and destination
        //     assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
        //     assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

        //     CommandBuffer commandBuffer;

        //     for (uint32_t i = 1; i < mipLevels; i++)
        //     {
        //         VkImageMemoryBarrier barrier0 = {};
        //         barrier0.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //         barrier0.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //         barrier0.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        //         barrier0.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        //         barrier0.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //         barrier0.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //         barrier0.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //         barrier0.image = image;
        //         barrier0.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //         barrier0.subresourceRange.baseMipLevel = i - 1;
        //         barrier0.subresourceRange.levelCount = 1;
        //         barrier0.subresourceRange.baseArrayLayer = baseArrayLayer;
        //         barrier0.subresourceRange.layerCount = layerCount;
        //         vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier0);

        //         VkImageBlit imageBlit = {};
        //         imageBlit.srcOffsets[1] = {int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1};
        //         imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //         imageBlit.srcSubresource.mipLevel = i - 1;
        //         imageBlit.srcSubresource.baseArrayLayer = baseArrayLayer;
        //         imageBlit.srcSubresource.layerCount = layerCount;
        //         imageBlit.dstOffsets[1] = {int32_t(extent.width >> i), int32_t(extent.height >> i), 1};
        //         imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //         imageBlit.dstSubresource.mipLevel = i;
        //         imageBlit.dstSubresource.baseArrayLayer = baseArrayLayer;
        //         imageBlit.dstSubresource.layerCount = layerCount;
        //         vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

        //         VkImageMemoryBarrier barrier1 = {};
        //         barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //         barrier1.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        //         barrier1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        //         barrier1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //         barrier1.newLayout = dstImageLayout;
        //         barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //         barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //         barrier1.image = image;
        //         barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //         barrier1.subresourceRange.baseMipLevel = i - 1;
        //         barrier1.subresourceRange.levelCount = 1;
        //         barrier1.subresourceRange.baseArrayLayer = baseArrayLayer;
        //         barrier1.subresourceRange.layerCount = layerCount;
        //         vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier1);
        //     }

        //     VkImageMemoryBarrier barrier = {};
        //     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //     barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        //     barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        //     barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        //     barrier.newLayout = dstImageLayout;
        //     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //     barrier.image = image;
        //     barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //     barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        //     barrier.subresourceRange.levelCount = 1;
        //     barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
        //     barrier.subresourceRange.layerCount = layerCount;
        //     vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        //     commandBuffer.SubmitIdle();
        // }
    }
}
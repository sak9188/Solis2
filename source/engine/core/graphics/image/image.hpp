#pragma once

#include "core/solis_core.hpp"
#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class SOLIS_CORE_API Image : public Object<Image>
        {
        public:
            Image() = default;
            virtual ~Image() = default;

            virtual void Update() = 0;

            static void CreateImage(VkImage &image, VkDeviceMemory &memory, const VkExtent3D &extent, VkFormat format, VkSampleCountFlagBits samples,
                                    VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels, uint32_t arrayLayers, VkImageType type);

            static void CreateImageSampler(VkSampler &sampler, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, uint32_t mipLevels);

            static void CreateImageView(const VkImage &image, VkImageView &imageView, VkImageViewType type, VkFormat format, VkImageAspectFlags imageAspect,
                                        uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer);

            // static void CreateMipmaps(const VkImage &image, const VkExtent3D &extent, VkFormat format, VkImageLayout dstImageLayout, uint32_t mipLevels,
            // uint32_t baseArrayLayer, uint32_t layerCount);

        protected:
            VkExtent3D extent;
            VkSampleCountFlagBits samples;
            VkImageUsageFlags usage;
            VkFormat format = VK_FORMAT_UNDEFINED;
            uint32_t mipLevels = 0;
            uint32_t arrayLayers;

            VkFilter filter;
            VkSamplerAddressMode addressMode;

            VkImageLayout layout;

            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
        };
    }
}

#pragma once

#include "volk.h"

#include "core/solis_core.hpp"
#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "core/graphics/vma.hpp"

namespace solis {
namespace graphics {
class Buffer;
class SOLIS_CORE_API Image : public Object<Image>
{
public:
    Image(VkExtent2D extent, VkFormat format, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage, VkFilter filter);
    Image(VkExtent3D extent, VkFormat format, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage, VkFilter filter);
    virtual ~Image();

    virtual void Update(const Buffer &buffer);

    virtual void TransitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t baseMipLevel = 0, uint32_t mipLevels = 1, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);

    // GetImageView
    VkImageView GetView() const
    {
        return mView;
    }

    // GetSampler
    VkSampler GetSampler() const
    {
        return mSampler;
    }

    static void CreateImage(VkImage &image, VkDeviceMemory &memory, const VkExtent3D &extent, VkFormat format, VkSampleCountFlagBits samples,
                            VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels, uint32_t arrayLayers, VkImageType type);

    static void CreateImageSampler(VkSampler &sampler, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, uint32_t mipLevels);

    static void CreateImageView(const VkImage &image, VkImageView &imageView, VkImageViewType type, VkFormat format, VkImageAspectFlags imageAspect,
                                uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer);

    // static void CreateMipmaps(const VkImage &image, const VkExtent3D &extent, VkFormat format, VkImageLayout dstImageLayout, uint32_t mipLevels,
    // uint32_t baseArrayLayer, uint32_t layerCount);
protected:
    VkExtent3D            mExtent;
    VkFilter              mFilter;
    VkSamplerAddressMode  mAddressMode;
    VkImageLayout         mLayout;
    VkSampleCountFlagBits mSamples;
    VkImageUsageFlags     mUsage;
    VkFormat              mFormat    = VK_FORMAT_UNDEFINED;
    uint32_t              mMipLevels = 0;
    uint32_t              mArrayLayers;

    VkImage        mImage         = VK_NULL_HANDLE;
    VkDeviceMemory mMemory        = VK_NULL_HANDLE;
    VkSampler      mSampler       = VK_NULL_HANDLE;
    VkImageView    mView          = VK_NULL_HANDLE;
    VmaAllocation  mVmaAllocation = VK_NULL_HANDLE;
};
}
} // namespace solis::graphics

#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis {
namespace graphics {
class SOLIS_CORE_API Buffer : public Object<Buffer>
{
public:
    enum class Status
    {
        Reset,
        Changed,
        Normal
    };

    enum class Type
    {
        None,
        Vertex,
        Index,
        Uniform
    };

    /**
     * Creates a new buffer with optional data.
     * @param size Size of the buffer in bytes.
     * @param usage Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer).
     * @param properties Memory properties for this buffer (i.e. device local, host visible, coherent).
     * @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over).
     */
    Buffer(const VkDeviceSize &size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, const void *data = nullptr);
    Buffer(Type type, const VkDeviceSize &size, const void *data = nullptr);
    virtual ~Buffer();

    void MapMemory(void **data) const;

    void UnmapMemory() const;

    VkDeviceSize GetSize() const
    {
        return mSize;
    }

    operator VkBuffer() const
    {
        return mBuffer;
    }

    const VkBuffer &GetBuffer() const
    {
        return mBuffer;
    }

    const VkDeviceMemory &GetBufferMemory() const
    {
        return mBufferMemory;
    }

    static uint32_t FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags &requiredProperties);

    // static void InsertBufferMemoryBarrier(const CommandBuffer &commandBuffer, const VkBuffer &buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
    // VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

protected:
    Type                  mType             = Type::None;
    VkDeviceSize          mSize             = 0;
    VkBuffer              mBuffer           = VK_NULL_HANDLE;
    VkDeviceMemory        mBufferMemory     = VK_NULL_HANDLE;
    VkMemoryPropertyFlags mMemoryProperties = 0;
};
}
} // namespace solis::graphics

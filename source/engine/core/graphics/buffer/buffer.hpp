#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/i_destroyable.hpp"

#include "volk.h"
#include "vk_mem_alloc.h"

namespace solis {
namespace graphics {
class SOLIS_CORE_API Buffer : public Object<Buffer>, public IDestroyable
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
        Stage,
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

    virtual void Destroy() override;

    // copy constructor
    // Buffer(const Buffer &other) = delete;

    // move constructor
    Buffer(Buffer &&other) noexcept
    {
        mType             = other.mType;
        mSize             = other.mSize;
        mBuffer           = other.mBuffer;
        mBufferMemory     = other.mBufferMemory;
        mMemoryProperties = other.mMemoryProperties;

        other.mType             = Type::None;
        other.mSize             = 0;
        other.mBuffer           = VK_NULL_HANDLE;
        other.mBufferMemory     = VK_NULL_HANDLE;
        other.mMemoryProperties = 0;
    }

    void Update(const void *data);

    void Update(const void *data, const size_t size);

    void MapMemory(void **data) const;

    void UnmapMemory() const;

    void CopyToBuffer(Buffer &buffer);

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
    VmaAllocation         mAllocation       = VK_NULL_HANDLE;

private:
    void Init(const VkDeviceSize &size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, const void *data);
};
}
} // namespace solis::graphics

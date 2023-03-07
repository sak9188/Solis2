#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"

#include "core/graphics/buffer/buffer.hpp"

namespace solis {
namespace graphics {

void Buffer::Init(const VkDeviceSize &size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, const void *data)
{
    // 如果是EXCLUESIVE的话，是不需要这个的
    // auto logicalDevice = Graphics::Get()->GetLogicalDevice();
    // auto graphicsFamily = logicalDevice->GetGraphicsFamily();
    // auto presentFamily = logicalDevice->GetPresentFamily();
    // auto computeFamily = logicalDevice->GetComputeFamily();

    // std::array queueFamily{graphicsFamily, presentFamily, computeFamily};

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size               = size;
    bufferInfo.usage              = usage;
    bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
    // bufferInfo.queueFamilyIndexCount = queueFamily.size();
    // bufferInfo.pQueueFamilyIndices = queueFamily.data();

    Graphics::CheckVk(vkCreateBuffer(*Graphics::Get()->GetLogicalDevice(), &bufferInfo, nullptr, &mBuffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*Graphics::Get()->GetLogicalDevice(), mBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize       = memRequirements.size;
    allocInfo.memoryTypeIndex      = FindMemoryType(memRequirements.memoryTypeBits, properties);

    Graphics::CheckVk(vkAllocateMemory(*Graphics::Get()->GetLogicalDevice(), &allocInfo, nullptr, &mBufferMemory));

    vkBindBufferMemory(*Graphics::Get()->GetLogicalDevice(), mBuffer, mBufferMemory, 0);
    if (data != nullptr)
    {
        void *mapped;
        MapMemory(&mapped);
        memcpy(mapped, data, static_cast<size_t>(size));
        UnmapMemory();
    }
}

Buffer::Buffer(const VkDeviceSize &size, const VkBufferUsageFlags &usage, const VkMemoryPropertyFlags &properties, const void *data) :
    mSize(size), mMemoryProperties(properties)
{
    Init(size, usage, properties, data);
}

// Buffer::Buffer(Buffer &&other) noexcept
// {
//     mBuffer           = other.mBuffer;
//     mBufferMemory     = other.mBufferMemory;
//     mSize             = other.mSize;
//     mType             = other.mType;
//     mMemoryProperties = other.mMemoryProperties;
//
//     other.mBuffer           = VK_NULL_HANDLE;
//     other.mBufferMemory     = VK_NULL_HANDLE;
//     other.mSize             = 0;
//     other.mType             = Type::Vertex;
//     other.mMemoryProperties = 0;
// }

void Buffer::Update(const void *data)
{
    void *mapped;
    MapMemory(&mapped);
    memcpy(mapped, data, static_cast<size_t>(mSize));
    UnmapMemory();
}

void Buffer::Update(const void *data, const size_t size)
{
    void *mapped;
    MapMemory(&mapped);
    memcpy(mapped, data, size);
    UnmapMemory();
}

Buffer::~Buffer()
{
    // TODO: for now we don't need to destroy buffer
    // vkDestroyBuffer(*Graphics::Get()->GetLogicalDevice(), mBuffer, nullptr);
    // vkFreeMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory, nullptr);
}

Buffer::Buffer(Type type, const VkDeviceSize &size, const void *data) :
    mType(type)
{
    VkBufferUsageFlags    usage      = 0;
    VkMemoryPropertyFlags properties = 0;

    switch (type)
    {
    case Type::Vertex:
        usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    // TODO: 这里可能有问题
    case Type::Index:
        usage      = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case Type::Uniform:
        usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    default:
        throw std::runtime_error("Invalid buffer type!");
    }

    Init(size, usage, properties, data);
}

void Buffer::MapMemory(void **data) const
{
    Graphics::CheckVk(vkMapMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory, 0, mSize, 0, data));
}

void Buffer::UnmapMemory() const
{
    vkUnmapMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory);
}

uint32_t Buffer::FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags &requiredProperties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(*Graphics::Get()->GetPhysicalDevice(), &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
}
} // namespace solis::graphics
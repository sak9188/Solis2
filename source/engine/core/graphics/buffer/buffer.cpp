#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"
#include "core/graphics/buffer/buffer.hpp"
#include "core/graphics/command/command_buffer.hpp"

#include "core/log/log.hpp"

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

#ifndef VMA_ENABLE
    Graphics::CheckVk(vkCreateBuffer(*Graphics::Get()->GetLogicalDevice(), &bufferInfo, nullptr, &mBuffer));
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*Graphics::Get()->GetLogicalDevice(), mBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize       = memRequirements.size;
    allocInfo.memoryTypeIndex      = FindMemoryType(memRequirements.memoryTypeBits, properties);
    Graphics::CheckVk(vkAllocateMemory(*Graphics::Get()->GetLogicalDevice(), &allocInfo, nullptr, &mBufferMemory));
    vkBindBufferMemory(*Graphics::Get()->GetLogicalDevice(), mBuffer, mBufferMemory, 0);
    // Graphics::Get()->AddBuffer(this);
#else
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage                   = VMA_MEMORY_USAGE_AUTO;
    allocCreateInfo.requiredFlags           = properties;
    if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    Graphics::CheckVk(vmaCreateBuffer(Graphics::Get()->GetAllocator(), &bufferInfo, &allocCreateInfo, &mBuffer, &mAllocation, nullptr));
#endif
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
    Destroy();
}

void Buffer::Destroy()
{
    if (IsDestroyed())
    {
        return;
    }

#ifndef VMA_ENABLE
    vkDestroyBuffer(*Graphics::Get()->GetLogicalDevice(), mBuffer, nullptr);
    vkFreeMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory, nullptr);
#else
    vmaDestroyBuffer(Graphics::Get()->GetAllocator(), mBuffer, mAllocation);
#endif
    mDestroyed = true;
}

Buffer::Buffer(Type type, const VkDeviceSize &size, const void *data) :
    mType(type), mSize(size)
{
    VkBufferUsageFlags    usage      = 0;
    VkMemoryPropertyFlags properties = 0;

    switch (type)
    {
    case Type::Vertex:
        usage      = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    // TODO: 这里可能有问题
    case Type::Index:
        usage      = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case Type::Stage:
        usage      = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case Type::Uniform:
        usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    default:
        throw std::runtime_error("Invalid buffer type!");
    }

    std::unique_ptr<Buffer> stageBuffer;
    if (type == Type::Vertex || type == Type::Index)
    {
        stageBuffer = std::make_unique<Buffer>(Type::Stage, size, data);
        data        = nullptr;
    }

    Init(size, usage, properties, data);

    if (stageBuffer)
    {
        stageBuffer->CopyToBuffer(*this);
    }

    Log::SDebug("Uniform Buffer created {}", (void *)this);
}

void Buffer::CopyToBuffer(Buffer &buffer)
{
    CommandBuffer commandBuffer;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = this->mSize;
    vkCmdCopyBuffer(commandBuffer, this->mBuffer, buffer.mBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer.GetCommandBuffer();

    auto device        = Graphics::Get()->GetLogicalDevice();
    auto graphicsQueue = device->GetGraphicsQueue();
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
}

void Buffer::MapMemory(void **data) const
{
#ifndef VMA_ENABLE
    Graphics::CheckVk(vkMapMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory, 0, mSize, 0, data));
#else
    vmaMapMemory(Graphics::Get()->GetAllocator(), mAllocation, data);
#endif
}

void Buffer::UnmapMemory() const
{
#ifndef VMA_ENABLE
    vkUnmapMemory(*Graphics::Get()->GetLogicalDevice(), mBufferMemory);
#else
    vmaUnmapMemory(Graphics::Get()->GetAllocator(), mAllocation);
#endif
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
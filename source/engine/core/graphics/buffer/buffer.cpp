#include "core/graphics/graphics.hpp"
#include "core/graphics/buffer/buffer.hpp"

namespace solis
{
    namespace graphics
    {
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
}
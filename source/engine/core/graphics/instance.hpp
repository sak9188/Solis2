#pragma once

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"
#include "vma/vk_mem_alloc.h"

namespace solis {
namespace graphics {
class SOLIS_CORE_API Instance : public Object<Instance>
{
    friend class Graphics;

public:
    friend VKAPI_ATTR VkBool32 VKAPI_CALL CallbackDebug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
    Instance();
    ~Instance();

    // #if USE_DEBUG_MESSENGER
    static VkResult FvkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                                    VkDebugUtilsMessengerEXT *pDebugMessenger);
    static void     FvkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator);

    static void FvkCmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set,
                                           uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites);

    static uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties *deviceMemoryProperties, const VkMemoryRequirements *memoryRequirements,
                                        VkMemoryPropertyFlags requiredProperties);

    operator const VkInstance &() const
    {
        return mInstance;
    }

    bool GetEnableValidationLayers() const
    {
        return mEnableValidationLayers;
    }
    const VkInstance &GetInstance() const
    {
        return mInstance;
    }

    static const vector<const char *> ValidationLayers;

private:
    bool                 CheckValidationLayerSupport() const;
    vector<const char *> GetExtensions() const;
    void                 CreateInstance();
    void                 CreateDebugMessenger();

    static void LogVulkanLayers(const vector<VkLayerProperties> &layerProperties);

    bool                     mEnableValidationLayers = false;
    VkInstance               mInstance               = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT mDebugMessenger         = VK_NULL_HANDLE;
};
}
} // namespace solis::graphics

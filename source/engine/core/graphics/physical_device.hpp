#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class Instance;

        class SOLIS_CORE_API PhysicalDevice : public Object<PhysicalDevice>
        {
            friend class Graphics;

        public:
            PhysicalDevice(const Instance &instance);
            virtual ~PhysicalDevice() = default;
            operator const VkPhysicalDevice &() const { return physicalDevice; }

            const VkPhysicalDevice &GetPhysicalDevice() const { return physicalDevice; }
            const VkPhysicalDeviceProperties &GetProperties() const { return properties; }
            const VkPhysicalDeviceFeatures &GetFeatures() const { return features; }
            const VkPhysicalDeviceMemoryProperties &GetMemoryProperties() const { return memoryProperties; }
            const VkSampleCountFlagBits &GetMsaaSamples() const { return msaaSamples; }

        private:
            VkPhysicalDevice ChoosePhysicalDevice(const std::vector<VkPhysicalDevice> &devices);
            VkSampleCountFlagBits GetMaxUsableSampleCount() const;

            static uint32_t ScorePhysicalDevice(const VkPhysicalDevice &device);
            static void LogVulkanDevice(const VkPhysicalDeviceProperties &physicalDeviceProperties, const std::vector<VkExtensionProperties> &extensionProperties);

            const Instance &instance;

            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            VkPhysicalDeviceProperties properties = {};
            VkPhysicalDeviceFeatures features = {};
            VkPhysicalDeviceMemoryProperties memoryProperties = {};
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        };
    }
}

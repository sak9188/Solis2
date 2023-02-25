#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class Instance;
        class PhysicalDevice;

        class SOLIS_CORE_API LogicalDevice : public Object<LogicalDevice>
        {
            friend class Graphics;

        public:
            LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice);
            ~LogicalDevice();

            operator const VkDevice &() const { return mLogicalDevice; }

            const VkDevice &GetLogicalDevice() const { return mLogicalDevice; }
            const VkPhysicalDeviceFeatures &GetEnabledFeatures() const { return mEnabledFeatures; }
            const VkQueue &GetGraphicsQueue() const { return mGraphicsQueue; }
            const VkQueue &GetPresentQueue() const { return mPresentQueue; }
            const VkQueue &GetComputeQueue() const { return mComputeQueue; }
            const VkQueue &GetTransferQueue() const { return mTransferQueue; }
            uint32_t GetGraphicsFamily() const { return mGraphicsFamily; }
            uint32_t GetPresentFamily() const { return mPresentFamily; }
            uint32_t GetComputeFamily() const { return mComputeFamily; }
            uint32_t GetTransferFamily() const { return mTransferFamily; }

            static const vector<const char *> DeviceExtensions;

        private:
            void CreateQueueIndices();
            void CreateLogicalDevice();

            const Instance &mInstance;
            const PhysicalDevice &mPhysicalDevice;

            VkDevice mLogicalDevice = VK_NULL_HANDLE;
            VkPhysicalDeviceFeatures mEnabledFeatures = {};

            VkQueueFlags mSupportedQueues = {};
            uint32_t mGraphicsFamily = 0;
            uint32_t mPresentFamily = 0;
            uint32_t mComputeFamily = 0;
            uint32_t mTransferFamily = 0;

            VkQueue mGraphicsQueue = VK_NULL_HANDLE;
            VkQueue mPresentQueue = VK_NULL_HANDLE;
            VkQueue mComputeQueue = VK_NULL_HANDLE;
            VkQueue mTransferQueue = VK_NULL_HANDLE;
        };
    }
}

#include <optional>

#include "core/graphics/graphics.hpp"
#include "core/graphics/instance.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"

#include "core/log/log.hpp"

namespace solis {
namespace graphics {

const vector<const char *> LogicalDevice::DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; // VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME

LogicalDevice::LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice) :
    mInstance(instance),
    mPhysicalDevice(physicalDevice)
{
    CreateQueueIndices();
    CreateLogicalDevice();
}

LogicalDevice::~LogicalDevice()
{
    Graphics::CheckVk(vkDeviceWaitIdle(mLogicalDevice));
    vkDestroyDevice(mLogicalDevice, nullptr);
}

void LogicalDevice::CreateQueueIndices()
{
    uint32_t deviceQueueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &deviceQueueFamilyPropertyCount, nullptr);

    vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &deviceQueueFamilyPropertyCount, deviceQueueFamilyProperties.data());

    std::optional<uint32_t> graphicsFamily, presentFamily, computeFamily, transferFamily;

    for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++)
    {
        // Check for graphics support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamily        = i;
            this->mGraphicsFamily = i;
            mSupportedQueues |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for presentation support.
        // VkBool32 presentSupport = VK_FALSE;
        // vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, *surface, &presentSupport);

        if (deviceQueueFamilyProperties[i].queueCount > 0 /*&& presentSupport*/)
        {
            presentFamily        = i;
            this->mPresentFamily = i;
        }

        // Check for compute support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            computeFamily        = i;
            this->mComputeFamily = i;
            mSupportedQueues |= VK_QUEUE_COMPUTE_BIT;
        }

        // Check for transfer support.
        if (deviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            transferFamily        = i;
            this->mTransferFamily = i;
            mSupportedQueues |= VK_QUEUE_TRANSFER_BIT;
        }

        if (graphicsFamily && presentFamily && computeFamily && transferFamily)
        {
            break;
        }
    }

    if (!graphicsFamily)
        throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
}

void LogicalDevice::CreateLogicalDevice()
{
    vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float                           queuePriorities[1] = {0.0f};

    if (mSupportedQueues & VK_QUEUE_GRAPHICS_BIT)
    {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
        graphicsQueueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex        = mGraphicsFamily;
        graphicsQueueCreateInfo.queueCount              = 1;
        graphicsQueueCreateInfo.pQueuePriorities        = queuePriorities;
        queueCreateInfos.emplace_back(graphicsQueueCreateInfo);
    }
    else
    {
        mGraphicsFamily = 0; // VK_NULL_HANDLE;
    }

    if (mSupportedQueues & VK_QUEUE_COMPUTE_BIT && mComputeFamily != mGraphicsFamily)
    {
        VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
        computeQueueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        computeQueueCreateInfo.queueFamilyIndex        = mComputeFamily;
        computeQueueCreateInfo.queueCount              = 1;
        computeQueueCreateInfo.pQueuePriorities        = queuePriorities;
        queueCreateInfos.emplace_back(computeQueueCreateInfo);
    }
    else
    {
        mComputeFamily = mGraphicsFamily;
    }

    if (mSupportedQueues & VK_QUEUE_TRANSFER_BIT && mTransferFamily != mGraphicsFamily && mTransferFamily != mComputeFamily)
    {
        VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
        transferQueueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        transferQueueCreateInfo.queueFamilyIndex        = mTransferFamily;
        transferQueueCreateInfo.queueCount              = 1;
        transferQueueCreateInfo.pQueuePriorities        = queuePriorities;
        queueCreateInfos.emplace_back(transferQueueCreateInfo);
    }
    else
    {
        mTransferFamily = mGraphicsFamily;
    }

    auto                     physicalDeviceFeatures = mPhysicalDevice.GetFeatures();
    VkPhysicalDeviceFeatures enabledFeatures        = {};

    // Enable sample rate shading filtering if supported.
    if (physicalDeviceFeatures.sampleRateShading)
        enabledFeatures.sampleRateShading = VK_TRUE;

    // Fill mode non solid is required for wireframe display.
    if (physicalDeviceFeatures.fillModeNonSolid)
    {
        enabledFeatures.fillModeNonSolid = VK_TRUE;

        // Wide lines must be present for line width > 1.0f.
        if (physicalDeviceFeatures.wideLines)
            enabledFeatures.wideLines = VK_TRUE;
    }
    else
    {
        Log::SWarning("Selected GPU does not support wireframe pipelines!\n");
    }

    if (physicalDeviceFeatures.samplerAnisotropy)
        enabledFeatures.samplerAnisotropy = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support sampler anisotropy!\n");

    if (physicalDeviceFeatures.textureCompressionBC)
        enabledFeatures.textureCompressionBC = VK_TRUE;
    else if (physicalDeviceFeatures.textureCompressionASTC_LDR)
        enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
    else if (physicalDeviceFeatures.textureCompressionETC2)
        enabledFeatures.textureCompressionETC2 = VK_TRUE;

    if (physicalDeviceFeatures.vertexPipelineStoresAndAtomics)
        enabledFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support vertex pipeline stores and atomics!\n");

    if (physicalDeviceFeatures.fragmentStoresAndAtomics)
        enabledFeatures.fragmentStoresAndAtomics = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support fragment stores and atomics!\n");

    if (physicalDeviceFeatures.shaderStorageImageExtendedFormats)
        enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support shader storage extended formats!\n");

    if (physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat)
        enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support shader storage write without format!\n");

    // enabledFeatures.shaderClipDistance = VK_TRUE;
    // enabledFeatures.shaderCullDistance = VK_TRUE;

    if (physicalDeviceFeatures.geometryShader)
        enabledFeatures.geometryShader = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support geometry shaders!\n");

    if (physicalDeviceFeatures.tessellationShader)
        enabledFeatures.tessellationShader = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support tessellation shaders!\n");

    if (physicalDeviceFeatures.multiViewport)
        enabledFeatures.multiViewport = VK_TRUE;
    else
        Log::SWarning("Selected GPU does not support multi viewports!\n");

    VkDeviceCreateInfo deviceCreateInfo   = {};
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos.data();
    if (mInstance.GetEnableValidationLayers())
    {
        deviceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(Instance::ValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = Instance::ValidationLayers.data();
    }
    deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(DeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures        = &enabledFeatures;
    Graphics::CheckVk(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mLogicalDevice));

    volkLoadDevice(mLogicalDevice);

    vkGetDeviceQueue(mLogicalDevice, mGraphicsFamily, 0, &mGraphicsQueue);
    vkGetDeviceQueue(mLogicalDevice, mPresentFamily, 0, &mPresentQueue);
    vkGetDeviceQueue(mLogicalDevice, mComputeFamily, 0, &mComputeQueue);
    vkGetDeviceQueue(mLogicalDevice, mTransferFamily, 0, &mTransferQueue);
}
}
} // namespace solis::graphics
#include "core/solis_engine.hpp"
#include "core/graphics/graphics.hpp"

#include "core/graphics/instance.hpp"
#include "core/graphics/physical_device.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/surface.hpp"
#include "core/graphics/swapchain.hpp"
#include "core/graphics/command/command_pool.hpp"
#include "core/graphics/pipeline/pipeline.hpp"
#include "core/graphics/render_pass.hpp"

namespace solis {
namespace graphics {
Graphics::Graphics()
{
    CheckVk(volkInitialize());
    mInstance       = std::make_unique<Instance>();
    mPhysicalDevice = std::make_unique<PhysicalDevice>(*mInstance);
    mLogicalDevice  = std::make_unique<LogicalDevice>(*mInstance, *mPhysicalDevice);

    this->CreateAllocator();
}

Graphics::~Graphics()
{
    vkDeviceWaitIdle(*mLogicalDevice);

    mSwapchains.clear();

    for (auto &renderPass : mRenderPasses)
    {
        renderPass->Destroy();
    }

    for (auto &pipeline : mPipelines)
    {
        pipeline->Destroy();
    }

    mCommandPool.reset();
    mPhysicalDevice.reset();
    mLogicalDevice.reset();
    mSurfaces.clear();
    mInstance.reset();
}

void Graphics::CreateAllocator()
{
    VmaVulkanFunctions vulkanFunctions    = {};
    vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

    vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties =
        vkGetPhysicalDeviceMemoryProperties;
    vulkanFunctions.vkGetDeviceBufferMemoryRequirements =
        vkGetDeviceBufferMemoryRequirements;
    vulkanFunctions.vkGetDeviceImageMemoryRequirements =
        vkGetDeviceImageMemoryRequirements;

    vulkanFunctions.vkGetBufferMemoryRequirements     = vkGetBufferMemoryRequirements;
    vulkanFunctions.vkGetImageMemoryRequirements      = vkGetImageMemoryRequirements;
    vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
    vulkanFunctions.vkGetImageMemoryRequirements2KHR  = vkGetImageMemoryRequirements2;

    vulkanFunctions.vkCreateBuffer                 = vkCreateBuffer;
    vulkanFunctions.vkDestroyBuffer                = vkDestroyBuffer;
    vulkanFunctions.vkAllocateMemory               = vkAllocateMemory;
    vulkanFunctions.vkFreeMemory                   = vkFreeMemory;
    vulkanFunctions.vkMapMemory                    = vkMapMemory;
    vulkanFunctions.vkUnmapMemory                  = vkUnmapMemory;
    vulkanFunctions.vkFlushMappedMemoryRanges      = vkFlushMappedMemoryRanges;
    vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vulkanFunctions.vkBindBufferMemory             = vkBindBufferMemory;
    vulkanFunctions.vkBindImageMemory              = vkBindImageMemory;
    vulkanFunctions.vkCreateImage                  = vkCreateImage;
    vulkanFunctions.vkDestroyImage                 = vkDestroyImage;
    vulkanFunctions.vkCmdCopyBuffer                = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice         = *mPhysicalDevice;
    allocatorInfo.device                 = *mLogicalDevice;
    allocatorInfo.instance               = *mInstance;
    allocatorInfo.vulkanApiVersion       = VK_API_VERSION_1_3;
    allocatorInfo.pVulkanFunctions       = &vulkanFunctions;

    CheckVk(vmaCreateAllocator(&allocatorInfo, &mVmaAllocator));
}

void Graphics::Init()
{
    mCommandPool = std::make_shared<CommandPool>(std::this_thread::get_id());
}

Swapchain *Graphics::CreateSurfaceSwapchain(const void *window, math::uvec2 extent)
{
    VkExtent2D vkextent{extent.x, extent.y};
    return this->CreateSurfaceSwapchain(window, vkextent);
}

Swapchain *Graphics::CreateSurfaceSwapchain(const void *window, VkExtent2D extent)
{
    mSurfaces.push_back(std::make_unique<Surface>(*mInstance, *mPhysicalDevice, *mLogicalDevice, window));
    mSwapchains.push_back(std::make_unique<Swapchain>(*mPhysicalDevice, *mLogicalDevice, *mSurfaces.back(), extent));
    return mSwapchains.back().get();
}

string Graphics::StringifyResultVk(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS:
        return "Success";
    case VK_NOT_READY:
        return "A fence or query has not yet completed";
    case VK_TIMEOUT:
        return "A wait operation has not completed in the specified time";
    case VK_EVENT_SET:
        return "An event is signaled";
    case VK_EVENT_RESET:
        return "An event is unsignaled";
    case VK_INCOMPLETE:
        return "A return array was too small for the result";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "A host memory allocation has failed";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "A device memory allocation has failed";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "Initialization of an object could not be completed for implementation-specific reasons";
    case VK_ERROR_DEVICE_LOST:
        return "The logical or physical device has been lost";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "Mapping of a memory object has failed";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "A requested layer is not present or could not be loaded";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "A requested extension is not supported";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "A requested feature is not supported";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "Too many objects of the type have already been created";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "A requested format is not supported on this device";
    case VK_ERROR_SURFACE_LOST_KHR:
        return "A surface is no longer available";
        // case VK_ERROR_OUT_OF_POOL_MEMORY:
        //	return "A allocation failed due to having no more space in the descriptor pool";
    case VK_SUBOPTIMAL_KHR:
        return "A swapchain no longer matches the surface properties exactly, but can still be used";
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "A surface has changed in such a way that it is no longer compatible with the swapchain";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "The display used by a swapchain does not use the same presentable image layout";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "A validation layer found an error";
    default:
        return "Unknown Vulkan error";
    }
}

void Graphics::CheckVk(VkResult result)
{
    if (result >= 0)
        return;

    // if (result == VK_ERROR_DEVICE_LOST)
    // {
    // VkDeviceFaultCountsEXT faultCounts;
    // VkDeviceFaultInfoEXT   faultInfo;
    // vkGetDeviceFaultInfoEXT(*Graphics::Get()->GetLogicalDevice(), &faultCounts, &faultInfo);
    // }

    auto failure = StringifyResultVk(result);

    throw std::runtime_error("Vulkan error: " + failure.toStdString());
}
}
} // namespace solis::graphics
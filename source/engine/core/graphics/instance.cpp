#include <iomanip>

#include "core/solis_core.hpp"
#include "core/base/const.hpp"

#include "core/log/log.hpp"

#include "core/graphics/instance.hpp"
#include "core/graphics/graphics.hpp"

#include "core/solis_engine.hpp"

#ifndef VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#define VK_EXT_DEBUG_UTILS_EXTENSION_NAME "VK_EXT_debug_utils"
#endif

namespace solis {
namespace graphics {
const vector<const char *> Instance::ValidationLayers = {"VK_LAYER_KHRONOS_validation"}; // "VK_LAYER_RENDERDOC_Capture"

VKAPI_ATTR VkBool32 VKAPI_CALL CallbackDebug(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        Log::SWarning(pCallbackData->pMessage);
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        Log::SInfo(pCallbackData->pMessage);
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        Log::SError(pCallbackData->pMessage);
    else
        Log::SInfo(pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult Instance::FvkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                   const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Instance::FvkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func)
        return func(instance, messenger, pAllocator);
}

void Instance::FvkCmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set,
                                          uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites)
{
    auto func = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR"));
    if (func)
        func(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

uint32_t Instance::FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties *deviceMemoryProperties, const VkMemoryRequirements *memoryRequirements,
                                       VkMemoryPropertyFlags requiredProperties)
{
    for (uint32_t i = 0; i < deviceMemoryProperties->memoryTypeCount; ++i)
    {
        if (memoryRequirements->memoryTypeBits & (1 << i))
        {
            if ((deviceMemoryProperties->memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
            {
                return i;
            }
        }
    }

    throw std::runtime_error("Couldn't find a proper memory type");
}

Instance::Instance()
{
#ifdef _DEBUG
    mEnableValidationLayers = true;
#endif

    CreateInstance();
    CreateDebugMessenger();
}

Instance::~Instance()
{
    FvkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

bool Instance::CheckValidationLayerSupport() const
{
    uint32_t instanceLayerPropertyCount;
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, nullptr);
    vector<VkLayerProperties> instanceLayerProperties(instanceLayerPropertyCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, instanceLayerProperties.data());

#ifdef _DEBUG
    LogVulkanLayers(instanceLayerProperties);
#endif

    for (const auto &layerName : ValidationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : instanceLayerProperties)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            Log::SError("Vulkan validation layer not found: \"{}\"", layerName);
            return false;
        }
    }

    return true;
}

vector<const char *> Instance::GetExtensions() const
{
    // Sets up the extensions.
    // auto [glfwExtensions, glfwExtensionsCount] = Windows::Get()->GetInstanceExtensions();

    auto extensions = Engine::Get()->CreateInfo().extensions;

    if (mEnableValidationLayers)
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    // extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    return extensions;
}

void Instance::CreateInstance()
{
    // const auto &engineVersion = Engine::Get()->GetVersion();
    // const auto &appVersion = Engine::Get()->GetApp()->GetVersion();
    // const auto &appName = Engine::Get()->GetApp()->GetName();

    Graphics::CheckVk(volkInitialize());

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // applicationInfo.pApplicationName = appName.c_str();
    // applicationInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
    applicationInfo.pEngineName   = EngineName;
    applicationInfo.engineVersion = VK_MAKE_VERSION(EngineVersionMajor, EngineVersionMinor, EngineVersionPatch);
    applicationInfo.apiVersion    = volkGetInstanceVersion() >= VK_API_VERSION_1_1 ? VK_API_VERSION_1_1 : VK_MAKE_VERSION(1, 0, 57);

    if (mEnableValidationLayers && !CheckValidationLayerSupport())
    {
        Log::SError("Validation layers requested, but not available!\n");
        mEnableValidationLayers = false;
    }

    auto extensions = GetExtensions();

    VkInstanceCreateInfo instanceCreateInfo    = {};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo        = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    if (mEnableValidationLayers)
    {
        debugUtilsMessengerCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsMessengerCreateInfo.pfnUserCallback = &CallbackDebug;
        instanceCreateInfo.pNext                      = static_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugUtilsMessengerCreateInfo);
    }

    Graphics::CheckVk(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));

#if VOLK_HEADER_VERSION >= 131
    volkLoadInstanceOnly(mInstance);
#else
    volkLoadInstance(instance);
#endif
}

void Instance::CreateDebugMessenger()
{
    if (!mEnableValidationLayers)
        return;

    // #if USE_DEBUG_MESSENGER
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    debugUtilsMessengerCreateInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType                        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback                    = &CallbackDebug;
    Graphics::CheckVk(FvkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsMessengerCreateInfo, nullptr, &mDebugMessenger));
}

void Instance::LogVulkanLayers(const vector<VkLayerProperties> &layerProperties)
{
    std::stringstream ss;

    ss << "Instance Layers: ";
    for (const auto &layer : layerProperties)
        ss << layer.layerName << ", ";

    ss << "\n\n";
    Log::SInfo(ss.str());
}
}
} // namespace solis::graphics

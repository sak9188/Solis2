#include "core/graphics/surface.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/instance.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/physical_device.hpp"
#include <cstdint>
#include <xcb/xcb.h>

namespace solis {
namespace graphics {
Surface::Surface(const Instance &instance, const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const void *window) :
    instance(instance),
    physicalDevice(physicalDevice),
    logicalDevice(logicalDevice), window(window)
{
// Creates the surface.
#ifdef __WIN__
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd      = (HWND)window;
    createInfo.hinstance = GetModuleHandle(nullptr);

    Graphics::CheckVk(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface));
#elif __LINUX__

#if defined(XLIB_WINDOW)
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy    = window;
    createInfo.window = window;

    Graphics::CheckVk(vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface));
#elif defined(XCB_WINDOW)
    VkXcbSurfaceCreateInfoKHR createInfo{};
    createInfo.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.connection = (xcb_connection_t *)instance.GetInstance();
    createInfo.window     = (uint64_t)window;

    Graphics::CheckVk(vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface));
#elif defined(WAYLAND_WINDOW)
    VkWaylandSurfaceCreateInfoKHR createInfo{};
    createInfo.sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.display = window;
    createInfo.surface = window;

    Graphics::CheckVk(vkCreateWaylandSurfaceKHR(instance, &createInfo, nullptr, &surface)
#elif defined(DIRECTFB_WINDOW)
    VkDirectFBSurfaceCreateInfoEXT createInfo{};
    createInfo.sType   = VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT;
    createInfo.dfb     = window;
    createInfo.surface = window;

    Graphics::CheckVk(vkCreateDirectFBSurfaceEXT(instance, &createInfo, nullptr, &surface)
#else
#error "Unsupported window system"
#endif

#elif __ANDROID__
    VkAndroidSurfaceCreateInfoKHR createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.window = window;

    Graphics::CheckVk(vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface));
#elif __MAC__
    VkMacOSSurfaceCreateInfoMVK createInfo{};
#endif

    Graphics::CheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

    uint32_t surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());

    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        format.format     = VK_FORMAT_B8G8R8A8_UNORM;
        format.colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        // Iterate over the list of available surface format and
        // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
        bool found_B8G8R8A8_UNORM = false;

        for (auto &surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                format.format        = surfaceFormat.format;
                format.colorSpace    = surfaceFormat.colorSpace;
                found_B8G8R8A8_UNORM = true;
                break;
            }
        }

        // In case VK_FORMAT_B8G8R8A8_UNORM is not available
        // select the first available color format
        if (!found_B8G8R8A8_UNORM)
        {
            format.format     = surfaceFormats[0].format;
            format.colorSpace = surfaceFormats[0].colorSpace;
        }
    }

    // Check for presentation support.
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, logicalDevice.GetPresentFamily(), surface, &presentSupport);

    if (!presentSupport)
        throw std::runtime_error("Present queue family does not have presentation support");
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
}
} // namespace solis::graphics

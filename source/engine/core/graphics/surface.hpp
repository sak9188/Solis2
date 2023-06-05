#pragma once

#include "volk.h"

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

namespace solis {
namespace graphics {
class Instance;
class PhysicalDevice;
class LogicalDevice;

class SOLIS_CORE_API Surface : public Object<Surface>
{
    friend class Graphics;

public:
    Surface(const Instance &instance, const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const void *window);
    Surface(const Instance &instance, const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, VkSurfaceKHR surface, const void *window);
    ~Surface();

    operator const VkSurfaceKHR &() const
    {
        return surface;
    }

    const VkSurfaceKHR &GetSurface() const
    {
        return surface;
    }
    const VkSurfaceCapabilitiesKHR &GetCapabilities() const
    {
        return capabilities;
    }
    const VkSurfaceFormatKHR &GetFormat() const
    {
        return format;
    }

private:
    void Init();

    const Instance       &instance;
    const PhysicalDevice &physicalDevice;
    const LogicalDevice  &logicalDevice;
    const void           *window;

    VkSurfaceKHR             surface      = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR capabilities = {};
    VkSurfaceFormatKHR       format       = {};
};
}
} // namespace solis::graphics

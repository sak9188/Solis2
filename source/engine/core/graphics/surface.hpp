#pragma once

#include "volk.h"

#include "core/solis_core.hpp"

namespace solis
{
	namespace graphics
	{
		class Instance;
		class PhysicalDevice;
		class LogicalDevice;

		class SOLIS_CORE_API Surface
		{
			friend class Graphics;

		public:
			Surface(const Instance &instance, const PhysicalDevice &physicalDevice, const LogicalDevice &logicalDevice, const void *window);
			~Surface();

			operator const VkSurfaceKHR &() const { return surface; }

			const VkSurfaceKHR &GetSurface() const { return surface; }
			const VkSurfaceCapabilitiesKHR &GetCapabilities() const { return capabilities; }
			const VkSurfaceFormatKHR &GetFormat() const { return format; }

		private:
			const Instance &instance;
			const PhysicalDevice &physicalDevice;
			const LogicalDevice &logicalDevice;
			const void *window;

			VkSurfaceKHR surface = VK_NULL_HANDLE;
			VkSurfaceCapabilitiesKHR capabilities = {};
			VkSurfaceFormatKHR format = {};
		};
	}
}

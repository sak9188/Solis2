#pragma once

#include <memory>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

// #include "core/graphics/instance.hpp"
// #include "core/graphics/physical_device.hpp"
// #include "core/graphics/logical_device.hpp"
// #include "core/graphics/surface.hpp"
// #include "core/graphics/swapchain.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class Instance;
        class PhysicalDevice;
        class LogicalDevice;
        class Surface;
        class Swapchain;
        class CommandPool;

        class SOLIS_CORE_API Graphics : public Object<Graphics>, public Module::Registrar<Graphics>
        {
            inline static const bool Registered = Register(Stage::Render);

        public:
            Graphics();
            virtual ~Graphics();

            virtual void Update() override {}

            // GetLogicalDevice()
            const LogicalDevice *GetLogicalDevice() const { return mLogicalDevice.get(); }

            // GetPhysicalDevice()
            const PhysicalDevice *GetPhysicalDevice() const { return mPhysicalDevice.get(); }

            const Instance *GetInstance() const { return mInstance.get(); }

            std::shared_ptr<CommandPool> GetCommandPool() const { return mCommandPool; }

            // CreateSurface
            void CreateSurfaceSwapchain(const void *window, math::uvec2 extent);

            // CreateSurface
            void CreateSurfaceSwapchain(const void *window, VkExtent2D extent);

            static string StringifyResultVk(VkResult result);

            static void CheckVk(VkResult result);

        private:
            std::unique_ptr<Instance> mInstance;
            std::unique_ptr<PhysicalDevice> mPhysicalDevice;
            std::unique_ptr<LogicalDevice> mLogicalDevice;
            std::shared_ptr<CommandPool> mCommandPool;

            vector<std::unique_ptr<Surface>> mSurfaces;
            vector<std::unique_ptr<Swapchain>> mSwapchains;
        };
    }
}

#pragma once

#include <memory>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class Instance;
        class PhysicalDevice;
        class LogicalDevice;

        class SOLIS_CORE_API Graphics : public Object<Graphics>, public Module::Registrar<Graphics>
        {
            inline static const bool Registered = Register(Stage::Render);

        public:
            Graphics() = default;
            virtual ~Graphics() = default;

            virtual void Update() override {}

            static string StringifyResultVk(VkResult result);
            static void CheckVk(VkResult result);

        private:
            std::unique_ptr<Instance> mInstance();
            std::unique_ptr<PhysicalDevice> mPhysicalDevice();
            std::unique_ptr<LogicalDevice> mLogicalDevice();
        };
    }
}

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class SOLIS_CORE_API CommandPool : public Object<CommandPool>
        {
        public:
            CommandPool(const std::thread::id &threadId = std::this_thread::get_id());
            virtual ~CommandPool();

            operator const VkCommandPool &() const { return commandPool; }

            const VkCommandPool &GetCommandPool() const { return commandPool; }
            const std::thread::id &GetThreadId() const { return threadId; }

        private:
            VkCommandPool commandPool = VK_NULL_HANDLE;
            std::thread::id threadId;
        };
    }
}
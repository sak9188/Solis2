#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/graphics/pipeline/pipeline.hpp"
#include "core/graphics/command/command_pool.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class SOLIS_CORE_API CommandBuffer : public Object<CommandBuffer>
        {
        public:
            /**
             * Creates a new command buffer.
             * @param begin If recording will start right away, if true {@link CommandBuffer#Begin} is called.
             * @param queueType The queue to run this command buffer on.
             * @param bufferLevel The buffer level.
             */
            explicit CommandBuffer(bool begin = true, VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            ~CommandBuffer();

            /**
             * Begins the recording state for this command buffer.
             * @param usage How this command buffer will be used.
             */
            void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
            void End();

            void BindPipeline(const Pipeline &pipeline) const
            {
                vkCmdBindPipeline(commandBuffer, pipeline.GetPipelineBindPoint(), pipeline.GetPipeline());
            }

            /**
             * Submits the command buffer to the queue and will hold the current thread idle until it has finished.
             */
            // void SubmitIdle();

            /**
             * Submits the command buffer.
             * @param waitSemaphore A optional semaphore that will waited upon before the command buffer is executed.
             * @param signalSemaphore A optional that is signaled once the command buffer has been executed.
             * @param fence A optional fence that is signaled once the command buffer has completed.
             */
            void Submit(const VkSemaphore &waitSemaphore = VK_NULL_HANDLE, const VkSemaphore &signalSemaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);

            operator const VkCommandBuffer &() const { return commandBuffer; }

            const VkCommandBuffer &GetCommandBuffer() const { return commandBuffer; }
            bool IsRunning() const { return running; }

        private:
            VkQueue GetQueue() const;
            std::shared_ptr<CommandPool> commandPool;

            VkQueueFlagBits queueType;
            VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
            bool running = false;
        };
    }
}
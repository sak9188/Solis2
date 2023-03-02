#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/graphics/pipeline/pipeline.hpp"
#include "core/graphics/command/command_pool.hpp"

#include "core/data/model.hpp"

#include "volk.h"

namespace solis
{
    namespace graphics
    {
        class Swapchain;
        class SOLIS_CORE_API CommandBuffer : public Object<CommandBuffer>
        {
        public:
            /**
             * Creates a new command buffer.
             * @param begin If recording will start right away, if true {@link CommandBuffer#Begin} is called.
             * @param queueType The queue to run this command buffer on.
             * @param bufferLevel The buffer level.
             */
            explicit CommandBuffer(VkQueueFlagBits queueType = VK_QUEUE_GRAPHICS_BIT, VkCommandBufferLevel bufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            ~CommandBuffer();

            void Begin(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
            void End();

            void BeginRenderPass(Swapchain &swapchain);
            void EndRenderPass();

            void BindPipeline(Pipeline *pipeline) const
            {
                vkCmdBindPipeline(commandBuffer, pipeline->GetPipelineBindPoint(), pipeline->GetPipeline());
            }

            void SetViewport(const VkViewport &viewport) const
            {
                vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            }

            void SetScissor(const VkRect2D &scissor) const
            {
                vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            }

            void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const
            {
                vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
            }

            void Draw(const Model &model) const
            {
                for (auto &mesh : model.GetMeshes())
                {
                    auto vertexBuffer = mesh->GetVertexBuffer();
                    assert(!vertexBuffer);
                    // 这里默认偏移量就是0, 目前还不清楚是否应该支持偏移量
                    VkBuffer vertexBuffers[] = {vertexBuffer->GetBuffer()};
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, 0);

                    auto indexBuffer = mesh->GetIndexBuffer();
                    assert(!indexBuffer);
                    // 这里默认偏移量就是0, 目前还不清楚是否应该支持偏移量
                    vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(commandBuffer, mesh->IndicesCount(), 1, 0, 0, 0);
                }
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
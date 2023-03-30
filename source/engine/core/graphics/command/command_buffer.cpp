#include "core/graphics/command/command_buffer.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/render_pass.hpp"
#include "core/graphics/swapchain.hpp"
#include "core/graphics/buffer/buffer.hpp"

namespace solis {
namespace graphics {
CommandBuffer::CommandBuffer(VkQueueFlagBits queueType, VkCommandBufferLevel bufferLevel) :
    commandPool(Graphics::Get()->GetCommandPool()),
    queueType(queueType)
{
    auto logicalDevice = Graphics::Get()->GetLogicalDevice();

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool                 = *commandPool;
    commandBufferAllocateInfo.level                       = bufferLevel;
    commandBufferAllocateInfo.commandBufferCount          = 1;
    Graphics::CheckVk(vkAllocateCommandBuffers(*logicalDevice, &commandBufferAllocateInfo, &commandBuffer));
}

CommandBuffer::~CommandBuffer()
{
    auto logicalDevice = Graphics::Get()->GetLogicalDevice();
    vkFreeCommandBuffers(*logicalDevice, commandPool->GetCommandPool(), 1, &commandBuffer);
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags usage)
{
    if (running)
        return;

    vkResetCommandBuffer(commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // beginInfo.flags = usage;
    Graphics::CheckVk(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    running = true;
}

void CommandBuffer::End()
{
    if (!running)
        return;

    Graphics::CheckVk(vkEndCommandBuffer(commandBuffer));
    running = false;
}

void CommandBuffer::BeginRenderPass(Swapchain &swapchain)
{
    if (!running)
        return;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass            = *swapchain.GetRenderPass();
    renderPassBeginInfo.framebuffer           = swapchain.GetActiveFrameBuffer();
    renderPassBeginInfo.renderArea.offset     = {0, 0};
    renderPassBeginInfo.renderArea.extent     = swapchain.GetExtent();

    VkClearValue clearValue             = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues    = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    mSwapchain = &swapchain;
}

void CommandBuffer::EndRenderPass()
{
    if (!running)
        return;

    vkCmdEndRenderPass(commandBuffer);
}

void CommandBuffer::BindPipeline(Pipeline *pipeline)
{
    assert(mSwapchain && "Swapchain must be set before binding pipeline");

    if (!running)
        return;

    vkCmdBindPipeline(commandBuffer, pipeline->GetPipelineBindPoint(), pipeline->GetPipeline());
    mPipeline = pipeline;
}

void CommandBuffer::BindPipelineDescriptorSet() const
{
    assert(mSwapchain && "Swapchain must be set before binding pipeline");
    assert(mPipeline && "Pipeline must be set before binding pipeline descriptor set");

    vkCmdBindDescriptorSets(commandBuffer, mPipeline->GetPipelineBindPoint(), mPipeline->GetPipelineLayout(),
                            0, 1, &(mPipeline->GetDescriptorSets()[mSwapchain->GetActiveImageIndex()]), 0, nullptr);
}

void CommandBuffer::Wait()
{
    // if (mSubmitSemaphore != VK_NULL_HANDLE)
    // {
    // VkSemaphoreWaitInfo waitInfo = {};
    // waitInfo.sType               = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    // waitInfo.pNext               = nullptr;
    // waitInfo.flags               = VK_SEMAPHORE_WAIT_ANY_BIT;
    // waitInfo.semaphoreCount      = 1;
    // waitInfo.pSemaphores         = &mSubmitSemaphore;
    // waitInfo.pValues             = nullptr;
    // vkWaitSemaphores(*Graphics::Get()->GetLogicalDevice(), &waitInfo, UINT64_MAX);
    // mSubmitSemaphore = VK_NULL_HANDLE;
    // }

    if (mSubmitFence != VK_NULL_HANDLE)
    {
        vkWaitForFences(*Graphics::Get()->GetLogicalDevice(), 1, &mSubmitFence, VK_TRUE, UINT64_MAX);
        // 让Swaphain去Reset这个Fence
        // vkResetFences(*Graphics::Get()->GetLogicalDevice(), 1, &mSubmitFence);
        mSubmitFence = VK_NULL_HANDLE;
    }
}

// void CommandBuffer::SubmitIdle()
// {
//     auto logicalDevice = Graphics::Get()->GetLogicalDevice();
//     auto queueSelected = GetQueue();

//     if (running)
//         End();

//     VkSubmitInfo submitInfo = {};
//     submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//     submitInfo.commandBufferCount = 1;
//     submitInfo.pCommandBuffers = &commandBuffer;

//     VkFenceCreateInfo fenceCreateInfo = {};
//     fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

//     VkFence fence;
//     Graphics::CheckVk(vkCreateFence(*logicalDevice, &fenceCreateInfo, nullptr, &fence));

//     Graphics::CheckVk(vkResetFences(*logicalDevice, 1, &fence));

//     Graphics::CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));

//     Graphics::CheckVk(vkWaitForFences(*logicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));

//     vkDestroyFence(*logicalDevice, fence, nullptr);
// }

void CommandBuffer::Submit(const VkSemaphore &waitSemaphore, const VkSemaphore &signalSemaphore, VkFence fence)
{
    auto logicalDevice = Graphics::Get()->GetLogicalDevice();
    auto queueSelected = GetQueue();

    if (running)
        End();

    VkSubmitInfo submitInfo       = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    if (waitSemaphore != VK_NULL_HANDLE)
    {
        // Pipeline stages used to wait at for graphics queue submissions.
        static VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submitInfo.pWaitDstStageMask  = &submitPipelineStages;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores    = &waitSemaphore;
    }

    if (signalSemaphore != VK_NULL_HANDLE)
    {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &signalSemaphore;

        // 记录提交的Semaphore
        this->mSubmitSemaphore = signalSemaphore;
    }

    if (fence != VK_NULL_HANDLE)
    {
        Graphics::CheckVk(vkResetFences(*logicalDevice, 1, &fence));
        this->mSubmitFence = fence;
    }

    Graphics::CheckVk(vkQueueSubmit(queueSelected, 1, &submitInfo, fence));
}

VkQueue CommandBuffer::GetQueue() const
{
    auto logicalDevice = Graphics::Get()->GetLogicalDevice();

    switch (queueType)
    {
    case VK_QUEUE_GRAPHICS_BIT:
        return logicalDevice->GetGraphicsQueue();
    case VK_QUEUE_COMPUTE_BIT:
        return logicalDevice->GetComputeQueue();
    default:
        return nullptr;
    }
}
}
} // namespace solis::graphics

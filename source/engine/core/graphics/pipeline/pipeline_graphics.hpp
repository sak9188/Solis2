#pragma once

#include "core/graphics/pipeline/pipeline.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/render_pass.hpp"

#include "core/graphics/buffer/buffer.hpp"

namespace solis {
namespace graphics {
struct UniformBufferObject
{
    math::mat4 mvp;
};

class SOLIS_CORE_API PipelineGraphics : public Pipeline
{
    friend class CommandBuffer;

public:
    PipelineGraphics() = default;
    virtual ~PipelineGraphics()
    {
        auto device = *Graphics::Get()->GetLogicalDevice();
        if (mDescriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
            mDescriptorSetLayout = VK_NULL_HANDLE;
        }

        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, mPipeline, nullptr);
            mPipeline = VK_NULL_HANDLE;
        }

        if (mPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
            mPipelineLayout = VK_NULL_HANDLE;
        }

        if (mDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
            mDescriptorPool = VK_NULL_HANDLE;
        }

        if (mDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
            mDescriptorSetLayout = VK_NULL_HANDLE;
        }
    };

    virtual bool    Build(const RenderPass &, size_t subpassIndex = 0) override;
    virtual Shader &GetShader() const override
    {
        return *mShader.get();
    }
    // virtual bool IsPushDescriptors() const override { return mIsPushDescriptors; }
    virtual const VkDescriptorSetLayout &GetDescriptorSetLayout() const override
    {
        return mDescriptorSetLayout;
    }
    virtual const VkDescriptorPool &GetDescriptorPool() const override
    {
        return mDescriptorPool;
    }
    virtual const VkPipeline &GetPipeline() const override
    {
        return mPipeline;
    }
    virtual const VkPipelineLayout &GetPipelineLayout() const override
    {
        return mPipelineLayout;
    }
    virtual const VkPipelineBindPoint &GetPipelineBindPoint() const override
    {
        return mPipelineBindPoint;
    }

    const void ResizeUniformBuffers(size_t size)
    {
        if (mUniformBuffers.size() != size)
        {
            mUniformBuffers.clear();
        }

        for (size_t i = 0; i < size; ++i)
        {
            // 这里应该知道每个buffer的大小
            Buffer buffer{Buffer::Type::Uniform, sizeof(UniformBufferObject)};
            mUniformBuffers.push_back(std::move(buffer));
        }
    }

public:
    std::unique_ptr<Shader> mShader = std::make_unique<Shader>();

    vector<Buffer> mUniformBuffers;

    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool      mDescriptorPool      = VK_NULL_HANDLE;

    VkPipeline          mPipeline          = VK_NULL_HANDLE;
    VkPipelineLayout    mPipelineLayout    = VK_NULL_HANDLE;
    VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
};
}
} // namespace solis::graphics

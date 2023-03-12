#pragma once

#include <unordered_set>

#include "core/graphics/pipeline/pipeline.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/render_pass.hpp"

#include "core/graphics/buffer/buffer.hpp"

namespace solis {
namespace graphics {

class SOLIS_CORE_API PipelineGraphics : public Pipeline
{
    friend class CommandBuffer;

public:
    PipelineGraphics();
    virtual ~PipelineGraphics();

    virtual void Destroy() override;

    virtual bool Build(const RenderPass &, size_t subpassIndex = 0) override;

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

    virtual const vector<VkDescriptorSet> &GetDescriptorSets() const override
    {
        return mDescriptorSets;
    }

    virtual Buffer &GetUniformBuffer(const Swapchain *swapchain, size_t index) override;

public:
    std::unique_ptr<Shader> mShader = std::make_unique<Shader>();

    VkDescriptorSetLayout      mDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool           mDescriptorPool      = VK_NULL_HANDLE;
    vector<VkDescriptorSet>    mDescriptorSets;
    std::unordered_set<size_t> mDirtyDescriptorSets;

    VkPipeline          mPipeline          = VK_NULL_HANDLE;
    VkPipelineLayout    mPipelineLayout    = VK_NULL_HANDLE;
    VkPipelineBindPoint mPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
};
}
} // namespace solis::graphics

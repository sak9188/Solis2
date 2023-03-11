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
    PipelineGraphics()
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount      = MaxFrameInFlight; // 这里要根据shader的descriptor数量来设置 * 3

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount              = 1;
        poolInfo.pPoolSizes                 = &poolSize;
        poolInfo.maxSets                    = MaxFrameInFlight; // todo: 这里要根据shader的descriptor数量来设置 *

        Graphics::CheckVk(vkCreateDescriptorPool(*Graphics::Get()->GetLogicalDevice(), &poolInfo, nullptr, &mDescriptorPool));

        // 创建Pipeline DescriptorSetLayout
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorCount    = 1; // todo: 这里要根据shader的descriptor数量来设置
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT; // 这里需要根据具体的shader来设置

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1; // todo: 这里应该通过反射来获取
        layoutInfo.pBindings    = &uboLayoutBinding;

        Graphics::CheckVk(vkCreateDescriptorSetLayout(*Graphics::Get()->GetLogicalDevice(), &layoutInfo, nullptr, &mDescriptorSetLayout));

        // Allocate DescriptorSet
        vector<VkDescriptorSetLayout> layouts(MaxFrameInFlight, mDescriptorSetLayout);
        VkDescriptorSetAllocateInfo   allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = mDescriptorPool;
        allocInfo.descriptorSetCount = layouts.size(); // todo: 这里要根据shader的descriptor数量来设置
        allocInfo.pSetLayouts        = layouts.data();

        mDescriptorSets.resize(MaxFrameInFlight); // 这里其实是有问题的 MaxFrameInFlight * Shader中的实际数量
        Graphics::CheckVk(vkAllocateDescriptorSets(*Graphics::Get()->GetLogicalDevice(), &allocInfo, mDescriptorSets.data()));
    };
    virtual ~PipelineGraphics()
    {
        auto device = *Graphics::Get()->GetLogicalDevice();

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

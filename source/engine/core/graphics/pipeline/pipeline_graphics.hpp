#pragma once

#include "core/graphics/pipeline/pipeline.hpp"

#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"
#include "core/graphics/render_pass.hpp"

namespace solis
{
    namespace graphics
    {
        class SOLIS_CORE_API PipelineGraphics : public Pipeline
        {
        public:
            PipelineGraphics() = default;
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
            virtual const Shader *GetShader() const override { return mShader.get(); }
            // virtual bool IsPushDescriptors() const override { return mIsPushDescriptors; }
            virtual const VkDescriptorSetLayout &GetDescriptorSetLayout() const override { return mDescriptorSetLayout; }
            virtual const VkDescriptorPool &GetDescriptorPool() const override { return mDescriptorPool; }
            virtual const VkPipeline &GetPipeline() const override { return mPipeline; }
            virtual const VkPipelineLayout &GetPipelineLayout() const override { return mPipelineLayout; }
            virtual const VkPipelineBindPoint &GetPipelineBindPoint() const override { return mPipelineBindPoint; }

        public:
            std::unique_ptr<Shader> mShader = std::make_unique<Shader>();

            VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;

            VkPipeline mPipeline = VK_NULL_HANDLE;
            VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
            VkPipelineBindPoint mPipelineBindPoint;
        };
    }
}

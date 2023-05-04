
#pragma once

#include "core/solis_core.hpp"
// #include "core/base/const.hpp"
#include "core/base/object.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/render_pass.hpp"
#include "core/graphics/buffer/buffer.hpp"

#include "volk.h"

namespace solis {
namespace graphics {
class Buffer;
class Swapchain;

/**
 * @brief 这个类主要是记录了各种RenderData
 *
 */
class SOLIS_CORE_API RenderData : public Object<RenderData>
{
};

class SOLIS_CORE_API Pipeline : public Object<Pipeline>,
                                public IDestroyable
{
public:
    Pipeline();
    virtual ~Pipeline();

    /**
     * @brief  Destroy the Pipeline object
     *
     */
    virtual void Destroy() override;

    /**
     * @brief 指定一个render graph中的pass的name来构建pipeline
     *
     * @param passname 如果是空那么就是默认的主要光照pass
     * @return true
     * @return false
     */
    virtual bool Build(const string &passname = "") = 0;

    /**
     * @brief 主动构建一个pipeline
     *
     * @param renderPass
     * @param subpassIndex
     * @return true
     * @return false
     */
    virtual bool Build(const RenderPass &renderPass, size_t subpassIndex = 0) = 0;

    /**
     * @brief Get the Shader object
     *
     * @return Shader&
     */
    virtual Shader &GetShader() const = 0;

    // virtual bool IsPushDescriptors() const = 0;
    virtual const VkDescriptorSetLayout   &GetDescriptorSetLayout() const = 0;
    virtual const VkDescriptorPool        &GetDescriptorPool() const      = 0;
    virtual const vector<VkDescriptorSet> &GetDescriptorSets() const      = 0;
    virtual const VkPipeline              &GetPipeline() const            = 0;
    virtual const VkPipelineLayout        &GetPipelineLayout() const      = 0;
    virtual const VkPipelineBindPoint     &GetPipelineBindPoint() const   = 0;

    // 先不考虑swapchain
    void            InitUniformBuffers(size_t size);
    virtual Buffer &GetUniformBuffer(size_t index);

    void Execute(CommandBuffer &commanderbuffer)
    {
        for (auto &data : mRenderDatas)
        {
            data.Execute(commanderbuffer);
        }
    }

private:
    // vector<Buffer> mUnionBuffers;
    // hash_map<size_t, vector<std::shared_ptr<Buffer>>> mUnionBufferMap{MaxSwapchain};
    vector<std::shared_ptr<Buffer>> mUnionBuffers;
    vector<RenderData>              mRenderDatas;
};
}
} // namespace solis::graphics
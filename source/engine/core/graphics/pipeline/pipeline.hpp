
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
class SOLIS_CORE_API Pipeline : public Object<Pipeline>, public IDestroyable
{
public:
    Pipeline();
    virtual ~Pipeline();

    virtual void Destroy() override;

    virtual bool    Build(const RenderPass &renderPass, size_t subpassIndex = 0) = 0;
    virtual Shader &GetShader() const                                            = 0;
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

private:
    // vector<Buffer> mUnionBuffers;
    // hash_map<size_t, vector<std::shared_ptr<Buffer>>> mUnionBufferMap{MaxSwapchain};

    vector<std::shared_ptr<Buffer>> mUnionBuffers;
};
}
} // namespace solis::graphics
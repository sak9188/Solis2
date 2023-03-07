
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/render_pass.hpp"
#include "core/graphics/buffer/buffer.hpp"

#include "volk.h"

namespace solis {
namespace graphics {
class Buffer;
class Swapchain;
class SOLIS_CORE_API Pipeline : public Object<Pipeline>
{
public:
    Pipeline()          = default;
    virtual ~Pipeline() = default;

    virtual bool    Build(const RenderPass &renderPass, size_t subpassIndex = 0) = 0;
    virtual Shader &GetShader() const                                            = 0;
    // virtual bool IsPushDescriptors() const = 0;
    virtual const VkDescriptorSetLayout &GetDescriptorSetLayout() const = 0;
    virtual const VkDescriptorPool      &GetDescriptorPool() const      = 0;
    virtual const VkPipeline            &GetPipeline() const            = 0;
    virtual const VkPipelineLayout      &GetPipelineLayout() const      = 0;
    virtual const VkPipelineBindPoint   &GetPipelineBindPoint() const   = 0;

    void InitUniformBuffers(const Swapchain *swapchain, size_t size, size_t count);

    Buffer &GetUniformBuffer(const Swapchain *swapchain, size_t index);

private:
    // vector<Buffer> mUnionBuffers;
    hash_map<size_t, vector<Buffer>> mUnionBufferMap{5};
};
}
} // namespace solis::graphics
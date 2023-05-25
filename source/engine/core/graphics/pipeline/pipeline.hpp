
#pragma once

#include "core/solis_core.hpp"
// #include "core/base/const.hpp"
#include "core/base/object.hpp"
#include "core/graphics/pipeline/shader.hpp"
#include "core/graphics/render_pass.hpp"
#include "core/graphics/buffer/buffer.hpp"

#include "core/data/mesh.hpp"

#include "volk.h"

namespace solis {
namespace graphics {

class Buffer;
class Swapchain;
class CommandBuffer;

/**
 * @brief 这个类主要是记录了各种RenderData
 *
 */
struct SOLIS_CORE_API RenderData : public Object<RenderData>
{
    std::unique_ptr<VkViewport> viewport = nullptr;
    std::unique_ptr<VkRect2D>   scissor  = nullptr;
    vector<VkDescriptorSet>     descriptorSets;
    vector<Mesh>               *meshes = nullptr;

    // copy constructor
    RenderData(const RenderData &other)
    {
        viewport = std::make_unique<VkViewport>(*other.viewport);
        scissor  = std::make_unique<VkRect2D>(*other.scissor);
        meshes   = other.meshes;
    }

    RenderData &operator=(const RenderData &other)
    {
        viewport = std::make_unique<VkViewport>(*other.viewport);
        scissor  = std::make_unique<VkRect2D>(*other.scissor);
        meshes   = other.meshes;
        return *this;
    }

    RenderData(RenderData &&other)
    {
        viewport = std::move(other.viewport);
        scissor  = std::move(other.scissor);
        meshes   = other.meshes;
    }
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
    // void            InitUniformBuffers(size_t size);
    // virtual Buffer &GetUniformBuffer(size_t index);

    void Execute(CommandBuffer &commanderbuffer);

private:
    // vector<Buffer> mUnionBuffers;
    // hash_map<size_t, vector<std::shared_ptr<Buffer>>> mUnionBufferMap{MaxSwapchain};
    vector<std::shared_ptr<Buffer>> mUnionBuffers;

    // TODO: 这里可以改成ObjectPool
    vector<RenderData> mRenderDatas;
};
}
} // namespace solis::graphics
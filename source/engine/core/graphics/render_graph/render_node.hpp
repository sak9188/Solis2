#pragma once

#include <memory>
#include <set>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

#include "core/graphics/pipeline/shader.hpp"

namespace solis {
namespace graphics {

class RenderGraph;

class SOLIS_CORE_API RenderNode : public Object<RenderNode>
{
public:
    RenderNode()          = default;
    virtual ~RenderNode() = default;

    class RenderNodeExecutor : public Object<RenderNodeExecutor>
    {
        VkRenderPass                                              renderPass = VK_NULL_HANDLE; // brow ref
        vector<Pipeline *>                                        pipelines;                   // brow ref
        vector<std::function<void(CommandBuffer &commandbuffer)>> callbacks;
    };

    string name;

    // 节点执行的时候需要的资源
    void RequireImage();

    void Execute()
    {
    }
};

class SOLIS_CORE_API RenderTargetNode : public Object<RenderTargetNode>
{
public:
    RenderTargetNode()          = default;
    virtual ~RenderTargetNode() = default;
};

// TODO: 这个类后面需要序列化，支持Python对象化，包括函数也要序列化
class SOLIS_CORE_API RenderGraphPipeline : public Object<RenderGraphPipeline>
{
public:
    vector<std::set<RenderNode>> renderNodes;

    // TODO: 这里需要任一个多线程组
    void Execute();
};

struct SOLIS_CORE_API GraphNode : public Object<GraphNode>
{
    GraphNode()          = default;
    virtual ~GraphNode() = default;

    string       name;
    size_t       index;
    RenderGraph *renderGraph = nullptr;
};

struct PassNode;

/**
 * @brief 资源节点
 * 如果资源节点，直接从一个PassNode的输出中获取
 * 那么这个资源节点就是一个RenderTarget
 */
struct SOLIS_CORE_API ResourceNode : public GraphNode
{
    enum class Type
    {
        RenderTarget,
        Texture,
    };

    ResourceNode()          = default;
    virtual ~ResourceNode() = default;

    VkFormat   format;
    math::vec2 size;

    // 作为Pass节点的输入
    vector<size_t> inputPasses;
    // 作为Pass节点的输出
    vector<size_t> outputResPasses;
};

struct SOLIS_CORE_API PassNode : public GraphNode
{
    PassNode()          = default;
    virtual ~PassNode() = default;

    vector<std::tuple<size_t, ResourceNode::Type>> inputs;
    vector<size_t>                                 outputs;
    // 默认情况下是-1, 代表没有被分配到layer
    int layer = -1;

    void AddInput(size_t index, ResourceNode::Type type);

    // 实际上是Shader引用PassNode, 这里做了解耦
    // dict_map<Shader::Type, string> shaderModuleName;
    bool IsValid()
    {
        // 输入可以默认为空， 但是输出不能为空
        if (inputs.empty() && outputs.empty())
            return false;

        return true;
    }

    bool CanMerge(const RenderGraph &graph);
};

}
} // namespace solis::graphics

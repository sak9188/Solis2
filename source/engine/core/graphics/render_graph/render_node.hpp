#pragma once

#include <memory>

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
    string                                                    name;
    VkRenderPass                                              renderPass = VK_NULL_HANDLE; // brow ref
    vector<Pipeline *>                                        pipelines;                   // brow ref
    vector<std::function<void(CommandBuffer &commandbuffer)>> callbacks;

    void Execute()
    {
    }
};

// TODO: 这个类后面需要序列化，支持Python对象化，包括函数也要序列化
class SOLIS_CORE_API RenderGraphPipeline : public Object<RenderGraphPipeline>
{
public:
    vector<RenderNode> renderNodes;

    // TODO: 这里需要任一个多线程组
    void Execute();
};

struct SOLIS_CORE_API GraphNode : public Object<GraphNode>
{
    GraphNode()          = default;
    virtual ~GraphNode() = default;

    string name;
    size_t index;
};

struct SOLIS_CORE_API PassNode : public GraphNode
{
    PassNode()          = default;
    virtual ~PassNode() = default;

    vector<size_t> inputs;
    vector<size_t> outputs;

    dict_map<Shader::Type, string> shaderModuleName;

    bool IsValid()
    {
        return shaderModuleName[Shader::Type::Vertex] != "" && shaderModuleName[Shader::Type::Fragment] != "";
    }

    bool CanMerge(const RenderGraph &graph, const PassNode &other);
};

struct SOLIS_CORE_API ResourceNode : public GraphNode
{
    ResourceNode()          = default;
    virtual ~ResourceNode() = default;

    VkFormat   format;
    math::vec2 size;
};

}
} // namespace solis::graphics

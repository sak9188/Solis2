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
class RenderPass;
class Pipeline;

class SOLIS_CORE_API RenderNode : public Object<RenderNode>
{
public:
    RenderNode()          = default;
    virtual ~RenderNode() = default;

    class RenderNodeExecutor : public Object<RenderNodeExecutor>
    {
        VkRenderPass       renderPass  = VK_NULL_HANDLE; // brow ref
        VkFramebuffer      framebuffer = VK_NULL_HANDLE;
        vector<Pipeline *> pipelines;                    // brow ref

    public:
        RenderNodeExecutor()          = default;
        virtual ~RenderNodeExecutor() = default;

        void Execute(CommandBuffer &commandbuffer)
        {
            for (auto pipeline : pipelines)
            {
                pipeline->Execute(commandbuffer);
            }
        }
    };

    // 节点索引
    size_t index = 0;

    // 节点名称
    string name;

    // VkRenderPass
    vector<VkAttachmentDescription>                 attachments;
    dict_map<string, vector<VkAttachmentReference>> attachmentReferences;
    vector<VkSubpassDescription>                    subpasses;
    vector<VkSubpassDependency>                     dependencies;

    // RenderPass
    // TODO: 后续需把这个RenderPass序列化
    RenderPass *renderPass = nullptr;

    // 节点执行器
    RenderNodeExecutor executor;

    // 节点执行的时候需要的资源
    void RequireImage();

    void Build();

    void Execute();
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
    // layerd nodes
    vector<vector<RenderNode>> renderNodes;

    dict_map<string, RenderNode *> renderNodeMap;

    /**
     * @brief Add a Pipeline object
     *
     * @param name
     * @param pipeline
     */
    void AddPipeline(const string &name, const Pipeline *pipeline)
    {
    }

    /**
     * @brief Get the Pass object
     *
     * @param name
     * @return RenderNode*
     */
    RenderNode *GetPassNode(const string &name)
    {
        return renderNodeMap[name];
    }

    // TODO: 这里需要任一个多线程组
    void
    Execute();
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

    bool                  isDepth       = false;
    VkFormat              format        = VK_FORMAT_B8G8R8A8_SRGB;
    math::vec2            size          = {1.0f, 1.0f};
    VkSampleCountFlagBits samples       = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp    load          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp   store         = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp    stencilLoad   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp   stencilStore  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout         initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout         finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // 作为Pass节点的输入
    vector<size_t> inputPasses;
    // 作为Pass节点的输出
    vector<size_t> outputResPasses;

    // bool CanAlias(const ResourceNode &other) const
    // {
    //     // TODO: 判断other节点所需要的内存是否是当前节点的内存range的子集
    //     return true;
    // }
};

struct SOLIS_CORE_API PassNode : public GraphNode
{
    PassNode()          = default;
    virtual ~PassNode() = default;

    vector<size_t> inputs;  // 作为资源节点的输入
    vector<size_t> outputs; // 作为资源节点的输出
    // 默认情况下是-1, 代表没有被分配到layer
    int layer = -1;
    // is SubPass
    bool   multipass    = false;
    size_t subPassIndex = 0;

    void AddInput(size_t index);
    void AddOuput(size_t index);

    // 实际上是Shader引用PassNode, 这里做了解耦
    // dict_map<Shader::Type, string> shaderModuleName;
    bool IsValid()
    {
        // 输入可以默认为空， 但是输出不能为空
        if (inputs.empty() && outputs.empty())
            return false;

        return true;
    }
    // bool CanMerge(const RenderGraph &graph);
};

}
} // namespace solis::graphics

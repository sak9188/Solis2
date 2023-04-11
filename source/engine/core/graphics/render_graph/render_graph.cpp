#include "core/graphics/render_graph/render_graph.hpp"

#include <set>

namespace std {
template <>
struct less<solis::graphics::PassNode *>
{
    constexpr bool operator()(const solis::graphics::PassNode *lhs, const solis::graphics::PassNode *rhs) const
    {
        return lhs->index < rhs->index;
    }
};
} // namespace std

namespace solis::graphics {

struct CompiledResourceNode
{
    ResourceNode *resourceNode = nullptr;             // 原始的资源节点，如果是alias那么就是别名的资源节点
    bool          isAlias      = false;               // 是否是别名
    VkFormat      targetFormat = VK_FORMAT_UNDEFINED; // 如果是别名，那么目标格式是什么
    math::vec2    targetSize   = math::vec2(0.0f);    // 如果是别名，那么目标大小是什么
    math::vec4    targetClear  = math::vec4(0.0f);    // 如果是别名，那么目标清除颜色是什么
};

struct CompilePassNode
{
    // 优先级
    size_t                                 priproty;
    PassNode                              *passNode = nullptr;
    dict_map<string, CompiledResourceNode> resourceNodes;
};

RenderGraph::RenderGraph()
{
    // 这里指代Swapchain中的图片
    auto backbuffer = this->AddBuildInResourceNode("backbuffer");

    // 这里是最后的Present节点
    this->AddBuildInPassNode("present").AddOuput(backbuffer.index);
}

void RenderGraph::SortPassNodes(vector<std::set<PassNode *>> &layerdNodes, vector<PassNode *> &passNodes, int layers)
{
    // 最大层数为32，超过就算循环依赖了
    assert(layers < 32 && "RenderGraph::SortPassNodes: node loop");

    for (int i = 0; i < passNodes.size(); ++i)
    {
        auto &passNode = passNodes[i];

        vector<PassNode *> tempPassNodes;
        // 找到输入的资源节点
        for (auto &output : passNode->outputs)
        {
            auto resourceNode = GetResourceNode(output);
            for (auto &passIndex : resourceNode->inputPasses)
            {
                auto &nextPassNode = mPassNodes[passIndex];
                nextPassNode.layer = std::max(nextPassNode.layer, passNode->layer + 1);
                tempPassNodes.push_back(&nextPassNode);
                if (layerdNodes.size() <= nextPassNode.layer)
                {
                    layerdNodes.resize(nextPassNode.layer + 1);
                }
                layerdNodes[nextPassNode.layer].insert(&nextPassNode);
            }
        }
        SortPassNodes(layerdNodes, tempPassNodes, layers + 1);
    }
}

RenderGraphPipeline RenderGraph::Compile()
{
    // 先检查一遍索引是否有改动
    dict_map<size_t, PassNode *> validPassNode;
    vector<PassNode *>           tempLayerNodes;
    vector<std::set<PassNode *>> layerdNodes;
    for (size_t i = 0; i < mPassNodes.size(); ++i)
    {
        auto &passNode = mPassNodes[i];
        if (passNode.index != i)
        {
            assert(false && "RenderGraph::Compile: pass node index error");
        }

        if (passNode.IsValid())
        {
            validPassNode.insert(std::make_pair(passNode.index, &passNode));
        }

        if (passNode.inputs.empty())
        {
            passNode.layer = 0;
            tempLayerNodes.push_back(&passNode);
        }
    }

    RenderGraphPipeline compileResult;

    // 这里把所有的PassNode按照依赖关系进行分层
    SortPassNodes(layerdNodes, tempLayerNodes);

    // 这里拿到已经合并好的PassNode
    // 开始编译成真正的RenderNode和Pipeline
    size_t passIndex = 0;
    for (auto &layerNodes : layerdNodes)
    {
        compileResult.renderNodes.emplace_back();
        auto &renderNodes = compileResult.renderNodes.back();
        for (auto &passNode : layerNodes)
        {
            renderNodes.emplace_back();
            auto &renderNode = renderNodes.back();

            renderNode.index = passIndex++;
            renderNode.name  = passNode->name;

            renderNode.subpasses.emplace_back();
            auto &subpass = renderNode.subpasses.back();

            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

            // TODO: 这里只考虑了单Pass的情况， 如果是多个Pass可能需要修改一下
            // 不过后面再说吧，这个多Subpass只在ARM芯片上有优化
            auto &colorAttachmentRefrences = renderNode.attachmentReferences["color"];
            auto &depthAttachmentRefrences = renderNode.attachmentReferences["depth"];

            for (auto &output : passNode->outputs)
            {
                auto resourceNode = GetResourceNode(output);

                renderNode.attachments.emplace_back();
                auto &attachment = renderNode.attachments.back();

                attachment.format         = resourceNode->format;
                attachment.samples        = resourceNode->samples;
                attachment.initialLayout  = resourceNode->initialLayout;
                attachment.finalLayout    = resourceNode->finalLayout;
                attachment.loadOp         = resourceNode->load;
                attachment.storeOp        = resourceNode->store;
                attachment.stencilLoadOp  = resourceNode->stencilLoad;
                attachment.stencilStoreOp = resourceNode->stencilStore;

                if (!resourceNode->isDepth)
                {
                    colorAttachmentRefrences.emplace_back();
                    auto &colorAttachment      = colorAttachmentRefrences.back();
                    colorAttachment.attachment = renderNode.attachments.size() - 1;
                    colorAttachment.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
                else
                {
                    assert(renderNode.attachmentReferences["depth"].empty() && "RenderGraph::Compile: depth attachment only one");

                    renderNode.attachmentReferences["depth"].emplace_back();
                    auto &depthAttachment      = depthAttachmentRefrences.back();
                    depthAttachment.attachment = renderNode.attachments.size() - 1;
                    depthAttachment.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                }
            }

            subpass.colorAttachmentCount    = colorAttachmentRefrences.size();
            subpass.pColorAttachments       = colorAttachmentRefrences.data();
            subpass.pDepthStencilAttachment = depthAttachmentRefrences.data();

            if (passNode->multipass)
            {
                assert(false && "RenderGraph::Compile: multipass not support yet");
                // renderNode.depenencies = passNode->dependencies;
            }
            else
            {
                static VkSubpassDependency single{
                    VK_SUBPASS_EXTERNAL,
                    0,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VK_DEPENDENCY_BY_REGION_BIT,
                };
                VkSubpassDependency copy = single;

                if (!depthAttachmentRefrences.empty())
                {
                    copy.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    copy.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    copy.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    copy.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                }

                renderNode.dependencies.push_back(copy);
            }

            // 此时已经构建出来RenderNode, 和实际的Subpass
            renderNode.Build();
        }
    }
    return std::move(compileResult);
}

PassNode &RenderGraph::AddPassNode(const string &name)
{
    if (mPassNodeMap.find(name) != mPassNodeMap.end())
    {
        assert(false && "RenderGraph::AddPassNode: pass node already exists");
    }

    mPassNodes.emplace_back();
    auto &node         = mPassNodes.back();
    node.name          = name;
    node.index         = mPassNodes.size() - 1;
    node.renderGraph   = this;
    mPassNodeMap[name] = &node;
    return node;
}

PassNode &RenderGraph::AddBuildInPassNode(const string &name)
{
    if (mPassNodeMap.find(name) != mPassNodeMap.end())
    {
        assert(false && "RenderGraph::AddPassNode: pass node already exists");
    }

    mBuildInPassNodes.emplace_back();
    auto &node         = mBuildInPassNodes.back();
    node.name          = "@" + name;
    node.index         = mBuildInPassNodes.size() - 1;
    node.renderGraph   = this;
    mPassNodeMap[name] = &node;
    return node;
}

ResourceNode &RenderGraph::AddResourceNode(const string &name)
{
    if (mResourceNodeMap.find(name) != mResourceNodeMap.end())
    {
        assert(false && "RenderGraph::AddResourceNode: resource node already exists");
    }

    mResourceNodes.emplace_back();
    auto &node             = mResourceNodes.back();
    node.name              = name;
    node.index             = mResourceNodes.size() - 1;
    node.renderGraph       = this;
    mResourceNodeMap[name] = &node;
    return node;
}

ResourceNode &RenderGraph::AddBuildInResourceNode(const string &name)
{
    if (mResourceNodeMap.find(name) != mResourceNodeMap.end())
    {
        assert(false && "RenderGraph::AddResourceNode: resource node already exists");
    }

    mBuildInResourceNodes.emplace_back();
    auto &node             = mBuildInResourceNodes.back();
    node.name              = "@" + name;
    node.index             = mBuildInResourceNodes.size() - 1;
    node.renderGraph       = this;
    mResourceNodeMap[name] = &node;
    return node;
}

PassNode *RenderGraph::GetPassNode(const string &name)
{
    auto it = mPassNodeMap.find(name);
    if (it != mPassNodeMap.end())
    {
        return it->second;
    }
    return nullptr;
}

ResourceNode *RenderGraph::GetResourceNode(const string &name)
{
    auto it = mResourceNodeMap.find(name);
    if (it != mResourceNodeMap.end())
    {
        return it->second;
    }
    return nullptr;
}

} // namespace solis::graphics
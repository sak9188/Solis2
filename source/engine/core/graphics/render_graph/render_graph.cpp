#include "core/graphics/render_graph/render_graph.hpp"

namespace solis::graphics {

struct CompileRenderPass
{
    // 合并的render pass
    vector<PassNode *> passNodes;
    // 合并的render pass需要最大RenderTarget的数量
    uint32_t maxRenderTargetCount = 0;
};

RenderGraph::RenderGraph()
{
    // 这里指代Swapchain中的图片
    auto backbuffer = this->AddBuildInResourceNode("backbuffer");

    // 这里是最后的Present节点
    this->AddBuildInPassNode("present")
        .inputs.push_back({backbuffer.index, ResourceNode::Type::Texture});
}

RenderGraphPipeline RenderGraph::Compile()
{
    // 先检查一遍索引是否有改动
    vector<PassNode *> validPassNode;
    for (size_t i = 0; i < mPassNodes.size(); ++i)
    {
        auto &passNode = mPassNodes[i];
        if (passNode.index != i)
        {
            assert(false && "RenderGraph::Compile: pass node index error");
        }

        if (passNode.IsValid())
        {
            validPassNode.push_back(&passNode);
        }
    }

    RenderGraphPipeline compileResult;

    // TODO: 这里对多个连续的PassNode进行Subpass合并
    vector<CompileRenderPass> combinePassNodes;
    for (size_t i = 0; i < validPassNode.size(); i++)
    {
        auto &passNode    = validPassNode[i];
        auto  combineNode = combinePassNodes.emplace_back();
        // 如果是第一个节点，直接添加
        if (combinePassNodes.empty())
        {
            combineNode.passNodes.push_back(passNode);
            continue;
        }

        // 如果是中间节点，需要判断是否可以合并
        // auto &lastPassNode = combinePassNodes.back().passNodes.back();
        if (passNode->CanMerge(*this))
        {
            combinePassNodes.back().passNodes.push_back(passNode);
        }
        else
        {
            combinePassNodes.emplace_back();
            combinePassNodes.back().passNodes.push_back(passNode);
        }
    }

    // 把Present节点单独放在最后
    auto combineNode = combinePassNodes.emplace_back();
    combineNode.passNodes.push_back(mPassNodeMap.find("@present")->second);

    // TODO: 这里最后需要计算到底需要多少个Image去渲染
    for (auto &passNode : combinePassNodes)
    {
    }

    // 这里拿到已经合并好的PassNode
    // 开始编译成真正的RenderNode和Pipeline
    for (auto &passNode : combinePassNodes)
    {
        compileResult.renderNodes.emplace_back();
        auto &renderNode = compileResult.renderNodes.back();
        for (auto &passNode : passNode.passNodes)
        {
            renderNode.name       = passNode->name;
            renderNode.renderPass = passNode->renderPass;
            renderNode.pipelines  = passNode->pipelines;
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
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

struct CompileRenderPass
{
    // 合并的render pass
    vector<std::set<PassNode *>> passNodes;
    // RenderTarget
    vector<ResourceNode *> renderTargets;
};

RenderGraph::RenderGraph()
{
    // 这里指代Swapchain中的图片
    auto backbuffer = this->AddBuildInResourceNode("backbuffer");

    // 这里是最后的Present节点
    this->AddBuildInPassNode("present")
        .inputs.push_back({backbuffer.index, ResourceNode::Type::Texture});
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
        for (auto &[input, type] : passNode->inputs)
        {
            auto resourceNode = GetResourceNode(input);
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

    // 这里最后需要计算到底需要多少个Image去渲染
    for (auto &layerNodes : layerdNodes)
    {
        for (auto &passNode : layerNodes)
        {
            for (auto &[intput, type] : passNode->inputs)
            {
                if (type == ResourceNode::Type::RenderTarget)
                {
                    auto resourceNode = GetResourceNode(intput);
                    if (resourceNode->renderTarget)
                    {
                        compileResult.renderTargets.push_back(resourceNode);
                    }
                }
            }
        }
    }

    // 把Present节点单独放在最后
    auto combineNode = combinePassNodes.emplace_back();
    combineNode.passNodes.push_back(mPassNodeMap.find("@present")->second);

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
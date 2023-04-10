#include "core/graphics/render_graph/render_node.hpp"
#include "core/graphics/render_graph/render_graph.hpp"

namespace solis::graphics {

void RenderNode::Execute()
{
}

void RenderGraphPipeline::Execute()
{
    for (auto &renderNode : renderNodes)
    {
        // 每一个节点执行之前 这个节点所依赖的节点都已经提交完毕
        // 因为是在单Queue下所以不会发生冲突
        renderNode.Execute();
    }
}

bool PassNode::CanMerge(const RenderGraph &graph)
{
    for (auto &[input, type] : inputs)
    {
        // 如果不是RenderTarget, 那么必然不能合批
        if (type != ResourceNode::Type::RenderTarget)
        {
            return false;
        }
    }
}

void PassNode::AddInput(size_t index, ResourceNode::Type type)
{
    inputs.emplace_back(index, type);
    auto resourceNode = renderGraph->GetResourceNode(index);
    resourceNode->inputPasses.push_back(this->index);

    if (type == ResourceNode::Type::RenderTarget)
    {
        resourceNode->outputResPasses.push_back(this->index);
        this->outputs.push_back(index);
    }
}

} // namespace solis::graphics

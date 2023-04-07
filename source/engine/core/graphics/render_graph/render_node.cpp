#include "core/graphics/render_graph/render_node.hpp"
#include "core/graphics/render_graph/render_graph.hpp"

namespace solis::graphics {

void RenderNode::Execute()
{
}

void RenderGraphPipelin::Execute()
{
    for (auto &renderNode : renderNodes)
    {
        // 每一个节点执行之前 这个节点所依赖的节点都已经提交完毕
        // 因为是在单Queue下所以不会发生冲突
        renderNode.Execute();
    }
}

bool PassNode::CanMerge(const RenderGraph &graph, const PassNode &other)
{
    graph.GetResourceNode()
}

} // namespace solis::graphics

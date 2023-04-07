#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/graphics/render_graph/render_node.hpp"
#include "core/graphics/pipeline/pipeline.hpp"
#include "core/graphics/render_pass.hpp"

namespace solis {
namespace graphics {

class Swapchain;
class CommandBuffer;

class SOLIS_CORE_API RenderGraph : public Object<RenderGraph>
{
public:
    RenderGraph();
    virtual ~RenderGraph() = default;

    RenderGraphPipeline Compile();

    void Clear();

    PassNode &AddPassNode(const string &name);

    ResourceNode &AddResourceNode(const string &name);

    PassNode *GetPassNode(const string &name);

    ResourceNode *GetResourceNode(const string &name);

private:
    // 添加内部的节点
    PassNode     &AddBuildInPassNode(const string &name);
    ResourceNode &AddBuildInResourceNode(const string &name);

    vector<PassNode>     mBuildInPassNodes;
    vector<ResourceNode> mBuildInResourceNodes;

    vector<PassNode>     mPassNodes;
    vector<ResourceNode> mResourceNodes;

    dict_map<string, PassNode *>     mPassNodeMap;
    dict_map<string, ResourceNode *> mResourceNodeMap;
};
}
} // namespace solis::graphics
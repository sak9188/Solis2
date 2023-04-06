#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace graphics {

class RenderNode;

class SOLIS_CORE_API RenderGraph : public Object<RenderGraph>
{
public:
    RenderGraph();
    virtual ~RenderGraph();

    void Compile();

    void Execute();

    void AddNode(const RenderNode &node);

    void RemoveNode(const RenderNode &node);

    void Clear();

private:
    // vector<RenderNode> mNodes;

    vector<PassNode>     mPassNodes;
    vector<ResourceNode> mResourceNodes;

    dict_map<string, size_t> mPassNodeMap;
    dict_map<string, size_t> mResourceNodeMap;
};
}
} // namespace solis::graphics
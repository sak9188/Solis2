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

    void AddNode(const RenderNode &node);

    void RemoveNode(const RenderNode &node);

    void Clear();

private:
    vector<RenderNode> mNodes;
};
}
} // namespace solis::graphics
#pragma once

#include <memory>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace graphics {
struct SOLIS_CORE_API RenderNode : public Object<RenderNode>
{
    RenderNode()          = default;
    virtual ~RenderNode() = default;

    string name;
};

struct SOLIS_CORE_API PassNode : public RenderNode
{
    PassNode()          = default;
    virtual ~PassNode() = default;

    vector<size_t> input;
    vector<size_t> output;
};

struct SOLIS_CORE_API ResourceNode : public RenderNode
{
    ResourceNode()          = default;
    virtual ~ResourceNode() = default;
};

}
} // namespace solis::graphics

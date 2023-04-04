#pragma once

#include <memory>

#include "core/solis_core.hpp"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace graphics {
class SOLIS_CORE_API RenderNode : public Object<RenderNode>
{
public:
    RenderNode()          = default;
    virtual ~RenderNode() = default;
};
}
} // namespace solis::graphics

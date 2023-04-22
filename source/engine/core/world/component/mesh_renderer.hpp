#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

#include "core/graphics/pipeline/material.hpp"

namespace solis {
namespace components {
class SOLIS_CORE_API MeshRenderer : Component<MeshRenderer>
{
public:
    virtual void OnAdd(GameObject *gameObject) override
    {
        // TODO: 从gameObject中获取mesh
    }

    virtual void OnRemove(GameObject *gameObject) override
    {
        // TODO: 从gameObject中获取mesh
    }

    // Material material;
};
}
} // namespace solis::components
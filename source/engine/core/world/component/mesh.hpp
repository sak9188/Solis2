
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

namespace solis {
namespace components {
class SOLIS_CORE_API Mesh : public Component<Mesh>
{
public:
    virtual void OnAdd(GameObject *gameObject) override
    {
        // TODO: 从gameObject中获取meshRenderer
    }

    virtual void OnRemove(GameObject *gameObject) override
    {
        // TODO: 从gameObject中获取meshRenderer
    }

    void SetMeshs(const vector<std::shared_ptr<Mesh>> &meshes)
    {
        mMeshes = meshes;
    }

private:
    vector<std::shared_ptr<Mesh>> mMeshes;
};
}
} // namespace solis::components
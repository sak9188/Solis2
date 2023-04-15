#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/system.hpp"
#include "core/world/component/renderable.hpp"

namespace solis {
class SOLIS_CORE_API RenderSystem : public System<RenderSystem>
{
public:
    virtual ~RenderSystem() = default;

    virtual void Update() override;

    /**
     * @brief 分配一个renderable，这个renderable将会被加入到整个系统中，如果不手动释放，那么将会在整个系统中被持续观察
     *
     * @return
     */
    components::Renderable *AllocRenderable();
};
} // namespace solis

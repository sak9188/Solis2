#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/system.hpp"

namespace solis {
class SOLIS_CORE_API RenderSystem : public System<RenderSystem>
{
public:
    virtual ~RenderSystem() = default;

    virtual void Update() override;
};
} // namespace solis

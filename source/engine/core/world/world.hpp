
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/world/world_base.hpp"

namespace solis {
class SOLIS_CORE_API World : public Object<World>, public Module::Registrar<World>
{
    inline static const bool Registered = Register(Stage::Normal);

public:
    World()          = default;
    virtual ~World() = default;

    virtual void Update() override
    {
        mMainWorld->Update();
    }

    void SetMainWorld(std::unique_ptr<WorldBase> &&world)
    {
        mMainWorld = std::move(world);
    }

    WorldBase &GetMainWorld() const
    {
        return *mMainWorld;
    }

private:
    std::unique_ptr<WorldBase> mMainWorld;
};
} // namespace solis
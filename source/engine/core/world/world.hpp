
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/world/world_base.hpp"
#include "core/events/events.hpp"
#include "core/events/event_define.hpp"

namespace solis {
class SOLIS_CORE_API World : public Object<World>, public Module::Registrar<World>, public EventHandler
{
    inline static const bool Registered = Register(Stage::Normal);

public:
    OBJECT_NEW_DELETE(World)
    World()
    {
        EVENT_REG(World, Start, EngineInitEvent);
    };
    virtual ~World() = default;

    bool Start(const EngineInitEvent &event)
    {
        mMainWorld->Start();

        // 只会被调用一次
        return false;
    }

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
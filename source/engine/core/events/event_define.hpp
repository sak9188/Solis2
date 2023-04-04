
#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/events/event_manager.hpp"

namespace solis {
struct EngineInitEvent : public Event
{
    EngineInitEvent()          = default;
    virtual ~EngineInitEvent() = default;
};

struct EngineInitPostEvent : public Event
{
    EngineInitPostEvent()          = default;
    virtual ~EngineInitPostEvent() = default;
};
} // namespace solis

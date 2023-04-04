
#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/events/event_manager.hpp"

#include <filesystem>
#include <iostream>
#include <tuple>

#define EVENT_REG(clazz, member, event) \
    events::Events::Get()->RegisterHandler<clazz, event, &clazz::member>(this)

#define EVENT_REG_PRIORITY(clazz, member, event, priority) \
    events::Events::Get()->RegisterHandler<clazz, event, &clazz::member>(this, priority)

#define EVENT_REGLATCH(clazz, up_event, down_event, event) \
    events::Events::Get()->RgisterLatchHandler<clazz, event, &clazz::up_event, &clazz::down_event>(this)

#define EVENT_REGLATCH_PRIORITY(clazz, up_event, down_event, event, priority) \
    events::Events::Get()->RgisterLatchHandler<clazz, event, &clazz::up_event, &clazz::down_event>(this, priority)

#define EVENT_SEND(event) \
    events::Events::Get()->SendEvent(event)

#define EVENT_POST(event) \
    events::Events::Get()->PostEvent(event)

namespace solis {
namespace events {

class SOLIS_CORE_API Events : public Object<Events>, public EventManager, public Module::Registrar<Events>
{
    inline static const bool Registered = Register(Stage::Always);

public:
    OBJECT_NEW_DELETE(Events)
    Events()          = default;
    virtual ~Events() = default;

    virtual void Update() override
    {
        for (auto &[type, event] : mNextFrameEvents)
        {
            EventManager::DispatchInline(type, *event);
            delete event;
        }
        mNextFrameEvents.clear();
    }

    // SENT_EVENT
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Event, T>, void>
    SendEvent(const T &e)
    {
        EventManager::DispatchInline(e);
    }

    // POST_EVENT
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Event, T>, void>
    PostEvent(const T &e)
    {
        Event *eptr = new T(e);
        mNextFrameEvents.emplace_back(std::make_tuple(ctti::type_id<T>().hash(), eptr));
    }

private:
    vector<std::tuple<uint64_t, Event *>> mNextFrameEvents;
};
}
} // namespace solis::events

#pragma once

#include <filesystem>
#include <iostream>
#include <tuple>

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/events/event_manager.hpp"

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

class SOLIS_CORE_API UpdateEvent : public Event
{
};

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

        for (auto &[pair, event] : mPostOnceEvents)
        {
            EventManager::DispatchInline(std::get<0>(pair), *event);
            delete event;
        }
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

    // 多次POST只会记录最后一次
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Event, T>, void>
    PostOnceEvent(const void *sender, const T &e)
    {
        uint64_t type = ctti::type_id<T>().hash();
        Event   *eptr = new T(e);
        auto     pair = std::make_tuple(type, sender);
        mPostOnceEvents.emplace(pair, eptr);
    }

private:
    vector<std::tuple<uint64_t, Event *>>                 mNextFrameEvents;
    dict_map<std::tuple<uint64_t, const void *>, Event *> mPostOnceEvents;
};
} // namespace events
} // namespace solis
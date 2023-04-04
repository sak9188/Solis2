#include "core/events/event_manager.hpp"

#include <algorithm>
#include <assert.h>

namespace solis {
EventManager::~EventManager()
{
    Dispatch();
    for (auto &[eventType, eventData] : mLatchedEvents)
    {
        for (auto &[pri, handlers] : eventData.mHandlers)
        {
            for (auto &handler : handlers)
            {
                DispatchDownEvents(eventData.mQueuedEvents, handler);
                // Before the event manager dies, make sure no stale EventHandler objects try to unregister themselves.
                handler.mUnregisterKey->ReleaseManagerReference();
            }
        }
    }
}

void EventManager::Dispatch()
{
    for (auto &[eventType, eventData] : mEvents)
    {
        auto &queued_events = eventData.mQueuedEvents;
        for (auto &[pri, handlers] : eventData.mHandlers)
        {
            auto itr = std::remove_if(begin(handlers), end(handlers), [&](const Handler &handler) {
                for (auto &event : queued_events)
                {
                    if (!handler.mem_fn(handler.mHandler, *event))
                    {
                        handler.mUnregisterKey->ReleaseManagerReference();
                        return true;
                    }
                }
                return false;
            });
            handlers.erase(itr, end(handlers));
        }
        queued_events.clear();
    }
}

void EventManager::DispatchEvent(vector<Handler> &handlers, const Event &e)
{
    auto itr = std::remove_if(begin(handlers), end(handlers), [&](const Handler &handler) -> bool {
        bool to_remove = !handler.mem_fn(handler.mHandler, e);
        if (to_remove)
            handler.mUnregisterKey->ReleaseManagerReference();
        return to_remove;
    });

    handlers.erase(itr, end(handlers));
}

void EventManager::DispatchUpEvents(vector<Event *> &up_events, const LatchHandler &handler)
{
    for (auto &event : up_events)
        handler.up_fn(handler.mHandler, *event);
}

void EventManager::DispatchDownEvents(vector<Event *> &down_events, const LatchHandler &handler)
{
    for (auto &event : down_events)
        handler.down_fn(handler.mHandler, *event);
}

void EventManager::LatchEventTypeData::FlushRecursiveHandlers()
{
    mHandlers[0].insert(end(mHandlers[0]), begin(mRecursiveHandlers), end(mRecursiveHandlers));
    mRecursiveHandlers.clear();
}

void EventManager::EventTypeData::FlushRecursiveHandlers()
{
    mHandlers[0].insert(end(mHandlers[0]), begin(mRecursiveHandlers), end(mRecursiveHandlers));
    mRecursiveHandlers.clear();
}

void EventManager::DispatchUpEvent(LatchEventTypeData &event_type, const Event &event)
{
    event_type.mDispatching = true;
    for (auto &[pri, handlers] : event_type.mHandlers)
    {
        for (auto &handler : handlers)
            handler.up_fn(handler.mHandler, event);
    }
    event_type.FlushRecursiveHandlers();
    event_type.mDispatching = false;
}

void EventManager::DispatchDownEvent(LatchEventTypeData &event_type, const Event &event)
{
    event_type.mDispatching = true;
    for (auto &[pri, handlers] : event_type.mHandlers)
    {
        for (auto &handler : handlers)
            handler.down_fn(handler.mHandler, event);
    }
    event_type.FlushRecursiveHandlers();
    event_type.mDispatching = false;
}

void EventManager::UnregisterHandler(EventHandler *handler)
{
    for (auto &[eventType, eventData] : mEvents)
    {
        for (auto &[pri, handlers] : eventData.mHandlers)
        {
            auto itr = std::remove_if(begin(handlers), end(handlers), [&](const Handler &h) -> bool {
                bool to_remove = h.mUnregisterKey == handler;
                if (to_remove)
                    h.mUnregisterKey->ReleaseManagerReference();
                return to_remove;
            });

            if (itr != end(handlers) && eventData.mDispatching)
                throw std::logic_error("Unregistering handlers while dispatching events.");

            if (itr != end(handlers))
                handlers.erase(itr, end(handlers));
        }
    }
}

void EventManager::UnregisterLatchHandler(EventHandler *handler)
{
    for (auto &[event_type, eventData] : mLatchedEvents)
    {
        for (auto &[pri, handlers] : eventData.mHandlers)
        {
            auto itr = std::remove_if(begin(handlers), end(handlers), [&](const LatchHandler &h) -> bool {
                bool to_remove = h.mUnregisterKey == handler;
                if (to_remove)
                    h.mUnregisterKey->ReleaseManagerReference();
                return to_remove;
            });

            if (itr != end(handlers))
                handlers.erase(itr, end(handlers));
        }
    }
}

void EventManager::DequeueLatched(uint64_t cookie)
{
    for (auto &[eventType, eventData] : mLatchedEvents)
    {
        auto &queued_events = eventData.mQueuedEvents;
        if (eventData.mEnqueueing)
            throw std::logic_error("Dequeueing latched while queueing events.");
        eventData.mEnqueueing = true;

        auto itr = std::remove_if(begin(queued_events), end(queued_events), [&](const Event *event) {
            bool signal = event->GetCookie() == cookie;
            if (signal)
                DispatchDownEvent(eventData, *event);
            return signal;
        });

        eventData.mEnqueueing = false;
        queued_events.erase(itr, end(queued_events));
    }
}

void EventHandler::ReleaseManagerReference()
{
    assert(mEventManagerRefCount > 0);
    assert(mEventManager);
    if (--mEventManagerRefCount == 0)
        mEventManager = nullptr;
}

void EventHandler::AddManagerReference(EventManager *manager)
{
    assert(!mEventManagerRefCount || manager == mEventManager);
    mEventManager = manager;
    mEventManagerRefCount++;
}

EventHandler::~EventHandler()
{
    if (mEventManager)
        mEventManager->UnregisterHandler(this);
    // Splitting the branch is significant since event manager can release its last reference in between.
    if (mEventManager)
        mEventManager->UnregisterLatchHandler(this);
    assert(mEventManagerRefCount == 0 && !mEventManager);
}
} // namespace solis

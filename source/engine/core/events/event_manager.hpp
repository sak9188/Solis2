#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "ctti/type_id.hpp"

namespace solis {
class Event;

template <typename Return, typename T, typename EventType, Return (T::*callback)(const EventType &e)>
Return MemberFunction(void *object, const Event &e)
{
    return (static_cast<T *>(object)->*callback)(static_cast<const EventType &>(e));
}

// #define GRANITE_EVENT_TYPE_HASH(x) ::Util::compile_time_fnv1(#x)
using EventType = uint64_t;

// #define GRANITE_EVENT_TYPE_DECL(x)                              \
    // enum class EventTypeWrapper : ::Granite::EventType          \
    // {                                                           \
        // type_id = GRANITE_EVENT_TYPE_HASH(x)                    \
    // };                                                          \
    // static inline constexpr ::Granite::EventType get_type_id()  \
    // {                                                           \
        // return ::Granite::EventType(EventTypeWrapper::type_id); \
    // }

class SOLIS_CORE_API Event : public Object<Event>
{
public:
    Event()          = default;
    virtual ~Event() = default;

    void SetCookie(uint64_t cookie)
    {
        mCookie = cookie;
    }

    uint64_t GetCookie() const
    {
        return mCookie;
    }

private:
    uint64_t mCookie = 0;
};

class EventManager;

// 需要在事件处理函数类中需要继承这个类，为了在析构时能够正确的释放EventManager的引用计数
class SOLIS_CORE_API EventHandler : public Object<EventHandler>
{
public:
    EventHandler(const EventHandler &)   = delete;
    void operator=(const EventHandler &) = delete;
    EventHandler()                       = default;
    ~EventHandler();

    void AddManagerReference(EventManager *manager);
    void ReleaseManagerReference();

private:
    EventManager *mEventManager         = nullptr;
    uint32_t      mEventManagerRefCount = 0;
};

class SOLIS_CORE_API EventManager : public Object<EventManager>
{
public:
    EventManager() = default;
    ~EventManager();

    template <typename T, typename... P>
    void Enqueue(P &&...p)
    {
        static constexpr auto type = ctti::type_id<T>().hash();
        auto                 &l    = mEvents[type];

        auto ptr = std::unique_ptr<Event>(new T(std::forward<P>(p)...));
        l.mQueuedEvents.emplace_back(std::move(ptr));
    }

    template <typename T, typename... P>
    uint64_t EnqueueLatched(P &&...p)
    {
        static constexpr auto type   = ctti::type_id<T>().hash();
        auto                 &l      = mLatchedEvents[type];
        auto                  ptr    = std::unique_ptr<Event>(new T(std::forward<P>(p)...));
        uint64_t              cookie = ++mCookieCounter;
        ptr->SetCookie(cookie);

        if (l.mEnqueueing)
            throw std::logic_error("Cannot enqueue more latched events while handling events.");
        l.mEnqueueing = true;

        auto *event = ptr.get();
        l.mQueuedEvents.emplace_back(std::move(ptr));
        DispatchUpEvent(l, *event);
        l.mEnqueueing = false;
        return cookie;
    }

    void DequeueLatched(uint64_t cookie);

    template <typename EventType>
    void DequeueAllLatched()
    {
        static constexpr auto type       = ctti::type_id<EventType>().hash();
        auto                 &event_type = mLatchedEvents[type];
        if (event_type.mEnqueueing)
            throw std::logic_error("Dequeueing latched while queueing events.");

        event_type.mEnqueueing = true;
        for (auto &event : event_type.mQueuedEvents)
            DispatchDownEvent(event_type, *event);
        event_type.mQueuedEvents.clear();
        event_type.mEnqueueing = false;
    }

    template <typename T>
    std::enable_if_t<std::is_base_of_v<Event, T>, void>
    DispatchInline(const T &t)
    {
        static constexpr auto type = ctti::type_id<T>().hash();
        auto                 &l    = mEvents[type];
        for (auto &[pri, h] : l.mHandlers)
            DispatchEvent(h, t);
    }

    void DispatchInline(uint64_t type_id, const Event &e)
    {
        auto &l = mEvents[type_id];
        for (auto &[pri, h] : l.mHandlers)
            DispatchEvent(h, e);
    }

    void Dispatch();

    template <typename T, typename EventType, bool (T::*mem_fn)(const EventType &)>
    void RegisterHandler(T *handler, uint32_t priority = 0)
    {
        handler->AddManagerReference(this);
        static constexpr auto type_id = ctti::type_id<T>().hash();
        auto                 &l       = mEvents[type_id];
        if (l.mDispatching)
            l.mRecursiveHandlers.push_back({MemberFunction<bool, T, EventType, mem_fn>, handler, handler});
        else
        {
            l.mHandlers[priority].push_back({MemberFunction<bool, T, EventType, mem_fn>, handler, handler});
        }
    }

    void UnregisterHandler(EventHandler *handler);

    template <typename T, typename EventType, void (T::*up_fn)(const EventType &), void (T::*down_fn)(const EventType &)>
    void RegisterLatchHandler(T *handler, uint32_t priority = 0)
    {
        handler->AddManagerReference(this);
        LatchHandler h{
            MemberFunction<void, T, EventType, up_fn>,
            MemberFunction<void, T, EventType, down_fn>,
            handler, handler};

        static constexpr auto type_id = EventType::get_type_id();
        auto                 &levents = mLatchedEvents[type_id];
        DispatchUpEvents(levents.mQueuedEvents, h);

        auto &l = mLatchedEvents[type_id];
        if (l.mDispatching)
            l.mRecursiveHandlers.push_back(h);
        else
        {
            l.mHandlers[priority].push_back(h);
        }
    }

    void UnregisterLatchHandler(EventHandler *handler);

private:
    struct Handler
    {
        bool (*mem_fn)(void *object, const Event &event);
        void         *mHandler;
        EventHandler *mUnregisterKey;
    };

    struct LatchHandler
    {
        void (*up_fn)(void *object, const Event &event);
        void (*down_fn)(void *object, const Event &event);
        void         *mHandler;
        EventHandler *mUnregisterKey;
    };

    struct EventTypeData
    {
        vector<Event *>                     mQueuedEvents;
        sort_map<uint32_t, vector<Handler>> mHandlers;
        vector<Handler>                     mRecursiveHandlers;
        bool                                mEnqueueing  = false;
        bool                                mDispatching = false;

        void FlushRecursiveHandlers();
    };

    struct LatchEventTypeData
    {
        vector<Event *>                          mQueuedEvents;
        sort_map<uint32_t, vector<LatchHandler>> mHandlers;
        vector<LatchHandler>                     mRecursiveHandlers;
        bool                                     mEnqueueing  = false;
        bool                                     mDispatching = false;

        void FlushRecursiveHandlers();
    };

    void DispatchEvent(vector<Handler> &handlers, const Event &e);
    void DispatchUpEvents(vector<Event *> &events, const LatchHandler &handler);
    void DispatchDownEvents(vector<Event *> &events, const LatchHandler &handler);
    void DispatchUpEvent(LatchEventTypeData &event_type, const Event &event);
    void DispatchDownEvent(LatchEventTypeData &event_type, const Event &event);

    dict_map<uint64_t, EventTypeData>      mEvents;
    dict_map<uint64_t, LatchEventTypeData> mLatchedEvents;
    uint64_t                               mCookieCounter = 0;
};
} // namespace solis


#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/events/events.hpp"

namespace solis {
template <typename T>
class SOLIS_CORE_API EventProperty : public Object<EventProperty<T>>, public EventHandler
{
public:
    OBJECT_NEW_DELETE(EventProperty<T>)

    EventProperty()
    {
        EVENT_REG(EventProperty<T>, OnUpdate, UpdateEvent);
    };

    EventProperty(uint32_t priority)
    {
        EVENT_REG_PRIORITY(EventProperty<T>, OnUpdate, UpdateEvent, priority);
    };

    virtual ~EventProperty() = default;

    // operator +=
    EventProperty &operator+=(std::function<bool(const T &)> &&function)
    {
        auto &dummy = mCallbacks;
        auto &vec   = dummy[static_cast<void *>(this)];
        vec.push_back(std::move(function));

        return *this;
    }

    // operator()
    void operator()(const T &value)
    {
        Invoke(value);
    }

    /**
     * @brief Dispatch is used to dispatch events immediately.
     *
     * @tparam T
     * @param value
     */
    void Invoke(const T &value)
    {
        auto &dummy = mCallbacks;
        auto &funs  = dummy[static_cast<void *>(this)];

        for (auto &function : funs)
        {
            function(value);
        }
    }

    /**
     * @brief  PostDispatch is used to dispatch events after the current frame is finished.
     *
     * @tparam T
     * @param value
     */
    void PostInvoke(const T &value)
    {
        mPostEvents.push_back(value);
    }

    /**
     * @brief PostOnceDispatch is used to dispatch events after the current frame is finished.
     *
     * @param value
     */
    void PostOnceInvoke(const T &value)
    {
        mPostOnceEvents = std::make_unique<T>(value);
    }

    /**
     * @brief  Reset is used to reset the event.
     *
     */
    void Reset()
    {
        mCallbacks.clear();
        mPostEvents.clear();
        mPostOnceEvents.reset();
    }

private:
    bool OnUpdate(const UpdateEvent &event)
    {
        if (mPostOnceEvents)
        {
            Invoke(*mPostOnceEvents);

            mPostOnceEvents.reset();
        }

        if (mPostEvents.empty())
        {
            return true;
        }

        for (auto &value : mPostEvents)
        {
            Invoke(value);
        }

        mPostEvents.clear();

        return true;
    }

    dict_map<void *, std::vector<std::function<bool(const T &)>>> mCallbacks;

    vector<T>          mPostEvents;
    std::unique_ptr<T> mPostOnceEvents;
};

} // namespace solis

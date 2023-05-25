#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/math/mat.hpp"

#include "core/events/event_define.hpp"
#include "core/events/event_property.hpp"

#include "core/data/component.hpp"

namespace solis {
// forward declare
namespace components {
class Transform;
}

struct TransformExpiredEvent : public Event
{
    TransformExpiredEvent(const components::Transform *transform) :
        transform(transform)
    {
    }
    const components::Transform *transform;
};

struct TransformChangedEvent : public Event
{
    TransformChangedEvent(const components::Transform *transform) :
        transform(transform)
    {
    }

    const components::Transform *transform;
};

namespace components {
class SOLIS_CORE_API Transform : public Component<Transform>
{
public:
    Transform() = default;

    virtual void OnDestroy() override
    {
        OnExpired.PostInvoke({this});

        OnExpired.Clear();
    }

    const math::vec3 &GetPosition() const
    {
        return mPosition;
    }

    const math::vec3 &GetRotation() const
    {
        return mRotation;
    }

    const math::vec3 &GetScale() const
    {
        return mScale;
    }

    void SetPosition(const math::vec3 &position)
    {
        mPosition = position;

        OnChanged.PostInvoke({this});
    }

    void SetRotation(const math::vec3 &rotation)
    {
        mRotation = rotation;

        OnChanged.PostInvoke({this});
    }

    void SetScale(const math::vec3 &scale)
    {
        mScale = scale;

        OnChanged.PostInvoke({this});
    }

    void LookAt(const math::vec3 &target, const math::vec3 &up = math::vec3(0.0f, 1.0f, 0.0f))
    {
        mViewMatrix = math::lookAt(mPosition, target, up);

        OnChanged.PostInvoke({this});
    }

    EventProperty<TransformExpiredEvent> OnExpired;

    EventProperty<TransformChangedEvent> OnChanged;

private:
    math::vec3 mPosition{0.0f, 0.0f, 0.0f};
    math::vec3 mRotation{0.0f, 0.0f, 0.0f};
    math::vec3 mScale{1.0f, 1.0f, 1.0f};

    math::mat4 mViewMatrix;
};
} // namespace components
} // namespace solis
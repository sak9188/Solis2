#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/events/event_define.hpp"

#include "core/data/component.hpp"

namespace solis {

// forward declare
namespace components {
class Transform;
}

struct TransformExpiredEvent : public Event
{
    const components::Transform *transform;
};

struct TransformChangedEvent : public Event
{
    const components::Transform *transform;
};

namespace components {
class SOLIS_CORE_API Transform : public Component<Transform>
{
public:
    Transform()  = default;
    ~Transform() = default;

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
    }

    void SetRotation(const math::vec3 &rotation)
    {
        mRotation = rotation;
    }

    void SetScale(const math::vec3 &scale)
    {
        mScale = scale;
    }

    // OnChanged +=

private:
    math::vec3 mPosition{0.0f, 0.0f, 0.0f};
    math::vec3 mRotation{0.0f, 0.0f, 0.0f};
    math::vec3 mScale{1.0f, 1.0f, 1.0f};
};
} // namespace components
} // namespace solis

#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

namespace solis {
namespace components {
class SOLIS_CORE_API Camera : public Component<Camera>
{
public:
    enum class Type
    {
        Perspective,
        Orthographic
    };

    enum class Projection
    {
        LeftHanded,
        RightHanded
    };

    Camera();
    virtual ~Camera(){};

    virtual void OnAdd(GameObject *gameObject) override;

    virtual void OnRemove(GameObject *gameObject) override;

    void SetNear(float fnear)
    {
        mNear = fnear;
    }

    void SetFar(float ffar)
    {
        mFar = ffar;
    }

    void SetFov(float fov)
    {
        mFov = fov;
    }

    void SetAspect(float aspect)
    {
        mAspect = aspect;
    }

    void SetType(Type type)
    {
        mType = type;
    }

    void SetProjection(Projection projection)
    {
        mProjection = projection;
    }

    float GetNear() const
    {
        return mNear;
    }

    float GetFar() const
    {
        return mFar;
    }

    float GetFov() const
    {
        return mFov;
    }

    float GetAspect() const
    {
        return mAspect;
    }

    Type GetType() const
    {
        return mType;
    }

    Projection GetProjection() const
    {
        return mProjection;
    }

private:
    float mNear   = 0.1f;
    float mFar    = 1000.0f;
    float mFov    = 45.0f;
    float mAspect = 1.0f;

    Type       mType       = Type::Perspective;
    Projection mProjection = Projection::RightHanded;
};
}
} // namespace solis::components
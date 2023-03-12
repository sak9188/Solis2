#pragma once

#include "core/solis_core.hpp"
#include "core/base/using.hpp"

namespace solis {
class SOLIS_CORE_API IDestroyable
{
public:
    IDestroyable()          = default;
    virtual ~IDestroyable() = default;

    virtual bool IsDestroyed() const
    {
        return mDestroyed;
    }
    virtual void Destroy() = 0;

protected:
    bool mDestroyed = false;
};
} // namespace solis


#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

namespace solis {

template <typename T>
class SOLIS_CORE_API System : public Object<System<T>>
{
public:
    virtual ~System() = default;

    System(const System &other)            = delete;
    System &operator=(const System &other) = delete;

    virtual void Start(){};
    virtual void Update(){};

    inline static T *Get()
    {
        static T instance;
        return &instance;
    }

private:
    System() = default;
};
} // namespace solis
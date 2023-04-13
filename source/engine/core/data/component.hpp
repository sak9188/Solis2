#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/base/ecs.hpp"

namespace solis {

struct SOLIS_CORE_API ComponentBase : public Object<ComponentBase>
{
public:
    virtual ~ComponentBase() = default;

    virtual uint64_t GetTypeId() = 0;
};

template <typename T>
struct SOLIS_CORE_API Component : public ComponentBase
{
public:
    Component() = default;
    virtual ~Component()
    {
        if (mInternalAlloc)
        {
            GetPool().FreeEntity(entityId);
        }
    };

    EntityID entityId;

    virtual uint64_t GetTypeId() override
    {
        return GetTypeId();
    }

    inline static std::enable_if_t<std::is_base_of_v<Component, T>, ObjectPool<T> &>
    GetPool()
    {
        static ObjectPool<T> pool;
        return pool;
    }

    inline static std::enable_if_t<std::is_base_of_v<Component, T>, T *>
    Get()
    {
        return GetPool().AllocEntity(&entityId);
    }

    inline static uint64_t GetTypeId()
    {
        return ctti::type_id<T>().hash();
    }

private:
    bool mInternalAlloc = false;
};

using ComponentMap = dict_map<uint64_t, ComponentBase *>;
} // namespace solis
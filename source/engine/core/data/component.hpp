#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/base/ecs.hpp"
#include "core/log/log.hpp"

#include "ctti/type_id.hpp"

namespace solis {

class SOLIS_CORE_API ComponentBase : public Object<ComponentBase>
{
public:
    ComponentBase()          = default;
    virtual ~ComponentBase() = default;

    virtual uint64_t GetTypeId() = 0;
};

class GameObject;

template <typename T>
class Component : public ComponentBase
{
    friend class ObjectPool<T>;

public:
    Component() = default;

    bool IsValid()
    {
        return mEntityID.IsValid();
    }

    void Destroy()
    {
        if (!IsValid())
        {
            Log::SWarning("Component already destroyed: {}", ctti::type_id<T>().name().str());
            return;
        }
        OnDestroy();
        GetPool().FreeEntity(mEntityID);
    }

    virtual uint64_t GetTypeId() override
    {
        return ctti::type_id<T>().hash();
    }

    inline static ObjectPool<T> &GetPool()
    {
        static ObjectPool<T> pool;
        return pool;
    }

    inline static T *Get()
    {
        EntityID id;
        auto     entity   = GetPool().AllocEntity(&id);
        entity->mEntityID = id;
        return entity;
    }

    EntityID mEntityID;

protected:
    virtual ~Component() = default;

    /**
     * @brief 因为组件不能被显示delete, 所以需要提供一个虚函数来实现析构
     *
     */
    virtual void OnDestroy(){};

    /**
     * @brief 当组件被添加到GameObject时调用
     *
     * @param gameObject
     */
    virtual void OnAdd(GameObject *gameObject){};

    /**
     * @brief 当组件从GameObject移除时调用
     *
     */
    virtual void OnRemove(GameObject *gameObject){};
};

using ComponentMap = dict_map<uint64_t, ComponentBase *>;
} // namespace solis
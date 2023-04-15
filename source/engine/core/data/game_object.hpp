#pragma once

#include <type_traits>

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

#include "core/log/log.hpp"
#include "ctti/type_id.hpp"

namespace solis {
class SOLIS_CORE_API GameObject : public Object<GameObject>
{
public:
    GameObject() = default;
    ~GameObject()
    {
        // 事件去触发Component的析构
    }

    /**
     * @brief 每种组件只能有一个, 如果需要添加多个，则用类去包装一下
     *     使用外部的组件池, 外部管理和释放
     *
     * @tparam T
     * @param component
     * @return void
     */
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Component<T>, T>, void>
    AddComponent(T *component)
    {
        if (HasComponent<T>())
        {
            Log::SWarning("GameObject add component already exists: {}", ctti::type_id<T>().name().str());
        }
        mCommponents[ctti::type_id<T>().hash()] = component;
    }

    /**
     * @brief 每种组件只能有一个, 如果需要添加多个，则用类去包装一下
     *       使用内置的组件池
     *
     * @tparam T
     * @param component
     * @return void
     */
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Component<T>, T>, T *>
    AddComponent()
    {
        if (HasComponent<T>())
        {
            Log::SWarning("GameObject add component already exists: {}", ctti::type_id<T>().name().str());
        }
        auto component                          = T::Get();
        mCommponents[ctti::type_id<T>().hash()] = component;
        return component;
    }

    // Remove Component
    template <typename T>
    std::enable_if_t<std::is_base_of_v<Component<T>, T>, void>
    RemoveComponent()
    {
        mCommponents.erase(ctti::type_id<T>().hash());
    }

    // Has Component
    template <typename T>
    bool HasComponent()
    {
        return mCommponents.find(ctti::type_id<T>().hash()) != mCommponents.end();
    }

private:
    ComponentMap            mCommponents;
    vector<ComponentBase *> mCommponentsList;
};
} // namespace solis
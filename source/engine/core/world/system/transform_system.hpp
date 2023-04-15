
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/system.hpp"
#include "core/data/game_object.hpp"

#include "core/world/component/transform.hpp"

namespace solis {
class SOLIS_CORE_API TransformSystem : public System<TransformSystem>, public EventHandler, public Object<TransformSystem>
{
public:
    OBJECT_NEW_DELETE(TransformSystem)

    virtual ~TransformSystem() = default;

    virtual void Update() override;

    /**
     * @brief 观察一个transform，如果transform被释放了，那么这个transform将会被移除
     *
     * @param transform
     */
    void Watch(components::Transform &transform);

    /**
     * @brief 取消观察一个transform
     *
     * @param transform
     */
    void UnWatch(components::Transform &transform);

    /**
     * @brief 判断一个transform是否被观察
     *
     * @param transform
     * @return true
     * @return false
     */
    bool IsWatched(components::Transform &transform);

    /**
     * @brief Set the Parent object, 如果parent被释放了
     *        那么base将会在整个系统中被移除，但是不会被释放
     *        除非重新加入到一个新的parent中, 或者手动释放
     *
     * @param base
     * @param parent
     */
    void SetParent(components::Transform &base, components::Transform &parent);

    /**
     * @brief Get the Parent object
     *
     * @param base
     * @return components::Transform*, return nullptr if not found
     */
    components::Transform *GetParent(components::Transform &base);

private:
    struct TransformNode
    {
        components::Transform     *transform = nullptr;
        TransformNode             *parent    = nullptr;
        std::list<TransformNode *> children;

        bool operator==(const TransformNode &other) const
        {
            return transform == other.transform && parent == other.parent && children == other.children;
        }
    };

    /**
     * @brief Get the Transform Node object
     *
     * @param transform
     * @return TransformNode&
     */
    TransformNode &GetTransformNode(components::Transform &transform);

    /**
     * @brief 这里使用事件去触发
     *
     * @param transform
     */
    bool OnTransformExpired(const TransformExpiredEvent &event);

    /**
     * @brief 这里使用事件去触发
     *
     * @param transform
     */
    bool OnTransformChanged(const TransformChangedEvent &event);

    // TODO: 这里可以做线性优化，不过预计需要花很多时间，先用list把
    TransformNode                                      mRoot;
    std::list<TransformNode>                           mTransformNodes;
    dict_map<components::Transform *, TransformNode *> mTransformNodeMap;
};
} // namespace solis

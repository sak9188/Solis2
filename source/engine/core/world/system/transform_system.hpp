
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/system.hpp"
#include "core/data/game_object.hpp"

#include "core/world/component/transform.hpp"

namespace solis {
class SOLIS_CORE_API TransformSystem : public System<TransformSystem>
{
public:
    virtual ~TransformSystem() = default;

    virtual void Update() override;

    /**
     * @brief 观察一个transform，如果transform被释放了，那么这个transform将会被移除
     *
     * @param transform
     */
    void Watch(components::Transform &transform);

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
     * @return components::Transform*
     */
    components::Transform *GetParent(components::Transform &base);

private:
    struct TransformNode
    {
        components::Transform     *transform = nullptr;
        TransformNode             *parent    = nullptr;
        std::list<TransformNode *> children;
    };

    TransformNode &GetTransformNode(components::Transform &transform);

    /**
     * @brief 这里使用事件去触发
     *
     * @param transform
     */
    void
    OnTransformExpired(components::Transform &transform);

    // TODO:(这里需要使用对象池，且对象池的增长策列为，线性增加)
    // 这里是线性内存(缓存并且拷贝std::list中的transform, 如果数量以及层级关系不变，那么这里就不会发生拷贝)
    // 这里由每帧的更新触发维护
    vector<components::Transform>                      mTransformCache;
    std::list<TransformNode>                           mTransformNodes;
    dict_map<components::Transform *, TransformNode *> mTransformNodeMap;
};
} // namespace solis

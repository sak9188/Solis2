#include "core/world/system/transform_system.hpp"

namespace solis {

void TransformSystem::Update()
{
    // TODO: 这里可以根据被观察的对象进行一个线性优化的过程
    // 不过目前这里使用的list+指针，所以目前还不需要进行到这一步
}

void TransformSystem::Watch(components::Transform &transform)
{
    if (mTransformNodeMap.find(&transform) != mTransformNodeMap.end())
    {
        return;
    }

    mTransformNodes.emplace_back(nullptr);
    mTransformNodes.back().transform = &transform;
    mTransformNodeMap[&transform]    = &mTransformNodes.back();

    mRoot.children.emplace_back(&mTransformNodes.back());

    transform.OnChanged += [this](const TransformChangedEvent &event) -> bool {
        return this->OnTransformChanged(event);
    };

    transform.OnExpired += [this](const TransformExpiredEvent &event) -> bool {
        return this->OnTransformExpired(event);
    };
}

void TransformSystem::UnWatch(components::Transform &transform)
{
    if (mTransformNodeMap.find(&transform) != mTransformNodeMap.end())
    {
        auto &node = GetTransformNode(transform);
        if (node.parent)
        {
            node.parent->children.remove(&node);
        }
        for (auto &child : node.children)
        {
            // TODO: 这里可能就变成了游离节点了
            // 或许这里可以做一个小小优化
            child->parent = nullptr;
        }
        mTransformNodes.remove(node);
        mTransformNodeMap.erase(&transform);
    }
}

bool TransformSystem::IsWatched(components::Transform &transform)
{
    return mTransformNodeMap.find(&transform) != mTransformNodeMap.end();
}

void TransformSystem::SetParent(components::Transform &base, components::Transform &parent)
{
    auto &node       = GetTransformNode(base);
    auto &parentNode = GetTransformNode(parent);

    if (node.parent)
    {
        node.parent->children.remove(&node);
    }

    node.parent = &parentNode;
    parentNode.children.emplace_back(&node);
}

components::Transform *TransformSystem::GetParent(components::Transform &base)
{
    if (mTransformNodeMap.find(&base) != mTransformNodeMap.end())
    {
        auto &node = mTransformNodeMap[&base];
        if (node->parent != nullptr)
        {
            return node->parent->transform;
        }
    }
    return nullptr;
}

TransformSystem::TransformNode &TransformSystem::GetTransformNode(components::Transform &transform)
{
    if (mTransformNodeMap.find(&transform) != mTransformNodeMap.end())
    {
        return *mTransformNodeMap[&transform];
    }

    mTransformNodes.emplace_back(&transform);
    auto &node                    = mTransformNodes.back();
    mTransformNodeMap[&transform] = &node;

    return node;
}

bool TransformSystem::OnTransformExpired(const TransformExpiredEvent &event)
{
    UnWatch(*const_cast<components::Transform *>(event.transform));

    return false;
}

bool TransformSystem::OnTransformChanged(const TransformChangedEvent &event)
{
    if (!IsWatched(*const_cast<components::Transform *>(event.transform)))
    {
        return false;
    }

    return false;
}
} // namespace solis
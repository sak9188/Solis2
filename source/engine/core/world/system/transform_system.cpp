#include "core/world/system/transform_system.hpp"

namespace solis {

void TransformSystem::Update()
{
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

TransformNode &TransformSystem::GetTransformNode(components::Transform &transform)
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

} // namespace solis
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/base/const.hpp"

#include "core/base/i_noncopyable.hpp"

namespace solis {
template <typename T>
class ObjectPoolNode : public Object<ObjectPoolNode<T>>, public INonCopyable
{
    OBJECT_NEW_DELETE(ObjectPoolNode)
public:
    ObjectPoolNode(size_t size) :
        mSize(size),
        mBack(0)
    {
        mEntities = static_cast<T *>(operator new[](size * sizeof(T)));
        mIndices  = static_cast<uint32_t *>(operator new[](size * sizeof(uint32_t)));

        for (uint32_t i = 0; i < size; ++i)
        {
            mIndices[i] = i;
        }
    }

    ~ObjectPoolNode()
    {
        operator delete[](mEntities);
        operator delete[](mIndices);
    }

    uint32_t AllocEntity(bool &success)
    {
        if (mBack == mSize)
        {
            success = false;
            return 0;
        }

        success = true;
        return mIndices[mBack++];
    }

    void FreeEntity(uint32_t index)
    {
        // 过大就纯纯不行
        if (index >= mSize)
        {
            return;
        }

        auto back = mBack;
        for (uint32_t i = 0; i < back; ++i)
        {
            if (mIndices[i] == index)
            {
                back--;
                std::swap(mIndices[i], mIndices[back]);
                break;
            }
        }
        mBack = back;
    }

    T &GetEntity(uint32_t index)
    {
        assert(index < mSize);
        return mEntities[index];
    }

private:
    T             *mEntities = nullptr;
    uint32_t      *mIndices  = nullptr;
    uint32_t       mBack;
    const uint32_t mSize;
};

/**
 * @brief 这里为啥是EntityID，而不是ObjectID呢？
 * 因为就是想单纯和ObjectID区分开来，这里的ID是从ObjectPool中分配出来的
 * 为啥ObjectPool不叫EntityPool呢？
 * 因为这是一个通用的ObjectPool，不仅仅是EntityPool
 * 但是分配出来的是一种特化的Object(也就是Entity)
 */
class EntityID : public Object<EntityID>
{
public:
    EntityID(uint32_t index, uint32_t poolIndex, uint64_t pool) :
        mIndex(index),
        mPoolIndex(poolIndex),
        mPool(pool)
    {
        assert(pool <= 0xFFFF);
        assert(poolIndex <= 0xFFFF);

        mUint64 = (mPool << 48) | (mPoolIndex << 32) | mIndex;
    }
    ~EntityID() = default;

    uint32_t GetIndex() const
    {
        return mIndex;
    }

    uint32_t GetPoolIndex() const
    {
        return mPoolIndex;
    }

    uint64_t GetPool() const
    {
        return mPool;
    }

    uint64_t GetUint64() const
    {
        return mUint64;
    }

    bool Empty()
    {
        return mUint64 == 0;
    }

    // operator ==
    bool operator==(const EntityID &other) const
    {
        return mUint64 == other.mUint64;
    }

private:
    // mPool(16) | mPoolIndex(16) | mIndex(32)
    uint64_t mUint64 = 0;
    // PoolNodeIndex | EntityIndex
    uint32_t mIndex = 0;
    // PoolIndex
    uint32_t mPoolIndex = 0;
    // Pool(uintptr_t)
    uint64_t mPool = 0;
};

template <typename T>
class ObjectPool : public Object<ObjectPool<T>>, public INonCopyable
{
public:
    enum class IncreaseType
    {
        Linear,      // 线性增长
        Exponential, // 指数增长
    };

    ObjectPool(IncreaseType type = IncreaseType::Exponential, size_t size = ObjectPoolInitSize) :
        mIncreaseType(type), mSize(size)
    {
        auto node = ObjectPoolNode<T>(mSize);
        mNodes.insert(node);
        mCache.push_back(--mNodes.end());
    };
    virtual ~ObjectPool() = default;

    T *AllocEntity(EntityID *id)
    {
        uint32_t nodeIndex = 0;
        bool     success   = false;

        for (auto &node : mNodes)
        {
            auto index = node.AllocEntity(success);
            if (success)
            {
                *id = EntityID(index, nodeIndex, reinterpret_cast<uintptr_t>(this));
                return &node.GetEntity(index);
            }
            nodeIndex++;
        }

        // 一些简单的增长策略
        switch (mIncreaseType)
        {
        case IncreaseType::Linear:
            mSize += ObjectPoolStepTimesSize;
            break;
        case IncreaseType::Exponential:
            mSize *= ObjectPoolStepTimesSize;
            break;

        default:
            assert(false && "ObjectPool::AllocEntity: unknown increase type");
            break;
        }

        auto node = ObjectPoolNode<T>(mSize);
        node.AllocEntity(success);

        mNodes.insert(node);
        mCache.push_back(--mNodes.end());

        assert(success);
        *id = EntityID(0, nodeIndex, reinterpret_cast<uintptr_t>(this));
        return &node.GetEntity(0);
    }

    // TODO: 这里需要做成那种可缩放的
    void FreeEntity(EntityID &id)
    {
        auto pool = id.GetPool();
        assert(pool == reinterpret_cast<uintptr_t>(this));

        auto nodeIndex = id.GetPoolIndex();
        auto index     = id.GetIndex();

        auto node = mCache[nodeIndex];
        node->FreeEntity(index);
    }

    T *GetEntity(EntityID &id)
    {
        auto pool = id.GetPool();
        assert(pool == reinterpret_cast<uintptr_t>(this));

        auto nodeIndex = id.GetPoolIndex();
        auto index     = id.GetIndex();

        auto node = mCache[nodeIndex];
        return node->GetEntity(index);
    }

    T *GetEntity(size_t index)
    {
        for (auto &node : mNodes)
        {
            if (index < node.Size())
            {
                return node.GetEntity(index);
            }

            // 过大就纯纯溢出了
            if (index > node.Size())
            {
                return nullptr;
            }

            index -= node.Size();
        }

        return nullptr;
    }

private:
    using NodeIterator = typename std::list<ObjectPoolNode<T>>::iterator;

    std::list<ObjectPoolNode<T>> mNodes;
    vector<NodeIterator>         mCache;

    IncreaseType mIncreaseType = IncreaseType::Exponential;
    uint32_t     mSize         = ObjectPoolInitSize;
};

/**
 * @brief 一个ObjectPool的Vector版本
 *
 * @tparam T
 */
template <typename T>
class ObjectPoolVector : public ObjectPool<T>
{
public:
    ObjectPoolVector() :
        ObjectPool(ObjectPool::IncreaseType::Linear);
    virtual ~ObjectPoolVector() = default;

    void Pushback(const T &ele)
    {
        EntityID id;

        // 这里不存储id
        T *ele = AllocEntity(&id);
        *ele   = ele;

        mVectorSize++;
    }

    size_t Size() const
    {
        return mVectorSize;
    }

    // operator []
    T &operator[](size_t index)
    {
        assert(index < mVectorSize);
        return *GetEntity(index);
    }

private:
    size_t mVectorSize = 0;
};

} // namespace solis
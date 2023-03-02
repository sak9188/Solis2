#pragma once

#include "core/solis_core.hpp"
#include "core/base/using.hpp"
#include "core/base/const.hpp"

#include "ctti/nameof.hpp"

namespace solis
{
    class SOLIS_CORE_API ObjectBase
    {
        // 最大 1000 堆上的对象

    public:
        ObjectBase() = default;
        virtual ~ObjectBase() = default;

        static void *Malloc(size_t size, string name, size_t count);

        static void Free(void *ptr);

#ifdef _DEBUG
        inline static size_t ObjectCount = 0;
        inline static size_t ObjectMemSize = 0;

        inline static hash_map<void *, string> ObjectNameMap{ObjectBase::GlobalObjectSize};
        inline static hash_map<void *, size_t> ObjectMap{ObjectBase::GlobalObjectSize};
        inline static hash_map<void *, string> ObjectTypeNameMap{ObjectBase::GlobalObjectSize};
        inline static hash_map<void *, size_t> ObjectArrayCountMap{ObjectBase::GlobalObjectSize};
#endif
    };

    // 使用宏定义来方便方便
#define OBJECT_NEW_DELETE(T)          \
    using Object<T>::operator new;    \
    using Object<T>::operator delete; \
    using Object<T>::operator new[];  \
    using Object<T>::operator delete[];

    template <typename T>
    class Object : public ObjectBase
    {
    public:
        void *operator new(size_t size)
        {
            auto name = ctti::nameof<T>().str();
            auto ptr = ObjectBase::Malloc(size, name, 1);
            return ptr;
        }

        // placement new
        void *operator new(size_t size, void *ptr)
        {
            return ptr;
        }

        void *operator new[](size_t size)
        {
            auto name = ctti::nameof<T>().str() + "[]";
            // 因为多继承的原因，有可能(没有继承Objevt<T>的情况下)无法准确计算数组的大小，所以这里只能用1来代替
            // 但运行时可以准确知道这个对象是什么类型，所以可以进一步知道这个数组有多少个元素
            auto ptr = ObjectBase::Malloc(size, name, 1);
            return ptr;
        }

        void operator delete(void *ptr)
        {
            return ObjectBase::Free(ptr);
        }

        void operator delete[](void *ptr)
        {
            return ObjectBase::Free(ptr);
        }

#ifdef _DEBUG
        // 对于某些特殊的对象， 我们可以给他一个名字来进行跟踪
        const string GetDebugObjectName() const
        {
            auto ptr = const_cast<Object<T> *>(this);
            if (ObjectBase::ObjectNameMap.find(ptr) != ObjectBase::ObjectNameMap.end())
            {
                return ObjectBase::ObjectNameMap.find(ptr)->second;
            }

            return "";
        }

        inline void SetDebugObjectName(const string &name)
        {
            ObjectBase::ObjectNameMap.insert(std::make_pair(this, name));
        }
#endif
    };
}
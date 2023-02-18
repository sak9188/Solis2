#pragma once

#include "core/solis_core.h"

#include "core/base/using.h"

namespace solis
{
    namespace core
    {
        class SOLIS_CORE_API ObjectBase
        {
            // 最大 1000 堆上的对象
            const static size_t GlobalObjectSize = 1000;

        public:
            ObjectBase() = default;
            virtual ~ObjectBase() = default;

            static void *Malloc(size_t size, string name, size_t count)
            {
                auto ptr = operator new(size *count);
#ifdef _DEBUG
                ObjectCount += count;
                ObjectMemSize += size;
                ObjectMap.insert(std::make_pair(ptr, size));
                ObjectTypeNameMap.insert(std::make_pair(ptr, name));
                if (count != 1) // 如果是数组
                {
                    ObjectArrayCountMap.insert(std::make_pair(ptr, count));
                    // ObjectArrayCountMap[ptr] = count;
                }
#endif
                return ptr;
            }

            static void Free(void *ptr)
            {
#ifdef _DEBUG
                if (ObjectArrayCountMap.find(ptr) != ObjectArrayCountMap.end())
                {
                    ObjectCount -= ObjectArrayCountMap.find(ptr)->second;
                    ObjectArrayCountMap.erase(ptr);
                }
                else
                {
                    ObjectCount--;
                }

                // 如果是走了Malloc的方法这里必定不会报错
                ObjectMemSize -= ObjectMap.find(ptr)->second;

                // 删除相关信息
                ObjectMap.erase(ptr);
                ObjectTypeNameMap.erase(ptr);
#endif
                return operator delete(ptr);
            }

            // TODO 输出所有的类信息

#ifdef _DEBUG
            inline static hash_map<void *, string> ObjectNameMap{ObjectBase::GlobalObjectSize};
#endif

        private:
#ifdef _DEBUG
            inline static size_t ObjectCount = 0;
            inline static size_t ObjectMemSize = 0;

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
            void *operator new(size_t size)
            {
                auto name = Core::GetTypeName(typeid(T).name());
                auto ptr = ObjectBase::Malloc(size, name, 1);
                return ptr;
            }

            void *operator new[](size_t size)
            {
                auto name = Core::GetTypeName(typeid(T).name()) + "[]";
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

                return string::empty();
            }

            inline void SetDebugObjectName(const string &name)
            {
                ObjectBase::ObjectNameMap.insert(std::make_pair(this, name));
            }
#endif
        };
    }
}
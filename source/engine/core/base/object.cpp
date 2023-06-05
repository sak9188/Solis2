#include "core/base/object.hpp"
#include "core/base/memory.hpp"

#include "core/base/using.hpp"
#include "mimalloc-2.0/mimalloc.h"

struct InitFuction
{
    InitFuction()
    {
        mi_option_set(mi_option_e::mi_option_verbose, 1);
        mi_option_set(mi_option_e::mi_option_show_stats, 1);
        mi_option_set(mi_option_e::mi_option_show_errors, 1);
    }
};

namespace solis {

hash_map<void *, string> ObjectBase::ObjectNameMap{GlobalObjectSize};
hash_map<void *, size_t> ObjectBase::ObjectMap{GlobalObjectSize};
hash_map<void *, string> ObjectBase::ObjectTypeNameMap{GlobalObjectSize};

void *ObjectBase::Malloc(size_t size, const string &name)
{
    static InitFuction initFuction;

    // auto ptr = operator new(size *count);
    auto ptr = mi_new(size);
#ifdef __DEBUG__
    ObjectCount += 1;
    ObjectMemSize += size;
    ObjectMap.insert(std::make_pair(ptr, size));
    ObjectTypeNameMap.insert(std::make_pair(ptr, name));
    // if (count != 1) // 如果是数组
    // {
    //     ObjectArrayCountMap.insert(std::make_pair(ptr, count));
    //     // ObjectArrayCountMap[ptr] = count;
    // }
#endif
    return ptr;
}

void *ObjectBase::MallocNoExcept(size_t size, const string &name)
{
    // auto ptr = operator new(size *count, std::nothrow);
    auto ptr = mi_new_nothrow(size);
#ifdef __DEBUG__
    ObjectCount += 1;
    ObjectMemSize += size;
    ObjectMap.insert(std::make_pair(ptr, size));
    ObjectTypeNameMap.insert(std::make_pair(ptr, name));
#endif
    return ptr;
}

void *ObjectBase::MallocAligned(size_t size, std::align_val_t align, const string &name)
{
    // auto ptr = operator new(size *count, align);
    auto ptr = mi_new_aligned(size, static_cast<size_t>(align));
#ifdef __DEBUG__
    ObjectCount += 1;
    ObjectMemSize += size;
    ObjectMap.insert(std::make_pair(ptr, size));
    ObjectTypeNameMap.insert(std::make_pair(ptr, name));
#endif
    return ptr;
}

void *ObjectBase::MallocAlignedNoExcept(size_t size, std::align_val_t align, const string &name)
{
    // auto ptr = operator new(size *count, align, std::nothrow);
    auto ptr = mi_new_aligned_nothrow(size, static_cast<size_t>(align));
#ifdef __DEBUG__
    ObjectCount += 1;
    ObjectMemSize += size;
    ObjectMap.insert(std::make_pair(ptr, size));
    ObjectTypeNameMap.insert(std::make_pair(ptr, name));
#endif
    return ptr;
}

void ObjectBase::Free(void *ptr)
{
#ifdef __DEBUG__
    // if (ObjectArrayCountMap.find(ptr) != ObjectArrayCountMap.end())
    // {
    // ObjectCount -= ObjectArrayCountMap.find(ptr)->second;
    // ObjectArrayCountMap.erase(ptr);
    // }
    // else
    // {
    // ObjectCount--;
    // }

    ObjectCount--;

    // 如果是走了Malloc的方法这里必定不会报错
    ObjectMemSize -= ObjectMap.find(ptr)->second;

    // 删除相关信息
    ObjectMap.erase(ptr);
    ObjectTypeNameMap.erase(ptr);
#endif
    // return operator delete(ptr);
    return mi_free(ptr);
}

void ObjectBase::FreeAligned(void *ptr, std::align_val_t align)
{
#ifdef __DEBUG__
    ObjectCount--;

    // 如果是走了Malloc的方法这里必定不会报错
    ObjectMemSize -= ObjectMap.find(ptr)->second;

    // 删除相关信息
    ObjectMap.erase(ptr);
    ObjectTypeNameMap.erase(ptr);
#endif
    return mi_free_aligned(ptr, static_cast<size_t>(align));
}

// FreeNoExcept
void ObjectBase::FreeNoExcept(void *ptr)
{
#ifdef __DEBUG__
    ObjectCount--;

    // 如果是走了Malloc的方法这里必定不会报错
    ObjectMemSize -= ObjectMap.find(ptr)->second;

    // 删除相关信息
    ObjectMap.erase(ptr);
    ObjectTypeNameMap.erase(ptr);
#endif
    return mi_free(ptr);
}

// FreeSize
void ObjectBase::FreeSize(void *ptr, size_t size)
{
#ifdef __DEBUG__
    ObjectCount--;

    // 如果是走了Malloc的方法这里必定不会报错
    ObjectMemSize -= ObjectMap.find(ptr)->second;

    // 删除相关信息
    ObjectMap.erase(ptr);
    ObjectTypeNameMap.erase(ptr);
#endif
    return mi_free_size(ptr, size);
}

// FreeAlignedNoExcept
void ObjectBase::FreeSizeAligned(void *ptr, size_t size, std::align_val_t align)
{
#ifdef __DEBUG__
    ObjectCount--;

    // 如果是走了Malloc的方法这里必定不会报错
    ObjectMemSize -= ObjectMap.find(ptr)->second;

    // 删除相关信息
    ObjectMap.erase(ptr);
    ObjectTypeNameMap.erase(ptr);
#endif
    return mi_free_aligned(ptr, static_cast<size_t>(align));
}

} // namespace solis

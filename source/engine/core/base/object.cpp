#include "core/base/object.hpp"
#include "core/base/memory.hpp"

#include "mimalloc-2.0/mimalloc-new-delete.h"

struct InitFuction
{
    InitFuction()
    {
        // mi_option_set(mi_option_e::mi_option_verbose, 1);
        // mi_option_set(mi_option_e::mi_option_show_stats, 1);
        mi_option_set(mi_option_e::mi_option_show_errors, 1);
    }
};

namespace solis {
void *ObjectBase::Malloc(size_t size, const string &name, size_t count)
{
    static InitFuction initFuction;

    auto ptr = operator new(size *count);
#ifdef _DEBUG
    ObjectCount += count;
    ObjectMemSize += size;
    ObjectMap.insert(std::make_pair(ptr, size));
    auto a = ObjectMap.size();
    ObjectTypeNameMap.insert(std::make_pair(ptr, name));
    if (count != 1) // 如果是数组
    {
        ObjectArrayCountMap.insert(std::make_pair(ptr, count));
        // ObjectArrayCountMap[ptr] = count;
    }
#endif
    return ptr;
}

void ObjectBase::Free(void *ptr)
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
} // namespace solis

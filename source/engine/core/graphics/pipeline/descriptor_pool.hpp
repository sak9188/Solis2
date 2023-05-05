#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis {
namespace graphics {

class SOLIS_CORE_API DescriptorPool : public Object<DescriptorPool>
{
public:
    OBJECT_NEW_DELETE(DescriptorPool)
    DescriptorPool(const vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets);
    ~DescriptorPool() = default;

private:
    vector<VkDescriptorPoolSize> mSizes;
    uint32_t                     mMaxSets;
    VkDescriptorPool             mPool;
    dict_map<uint64_t, uint32_t> mSets;
};

}
} // namespace solis::graphics
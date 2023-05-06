#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"

#include "volk.h"

namespace solis {
namespace graphics {

class LogicalDevice;

class SOLIS_CORE_API DescriptorPool : public Object<DescriptorPool>
{
public:
    OBJECT_NEW_DELETE(DescriptorPool)
    DescriptorPool(const vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets);
    ~DescriptorPool();

    /**
     * @brief 分配一个descriptor set
     *
     * @param layout
     * @return VkDescriptorSet
     */
    VkDescriptorSet Allocate(const VkDescriptorSetLayout &layout);

    /**
     * @brief 对于多帧的情况，直接分配最大异步帧数的descriptor set
     *
     * @param layout
     * @return vector<VkDescriptorSet>
     */
    vector<VkDescriptorSet> AllocateWithFrames(const VkDescriptorSetLayout &layout);

    VkDescriptorPool GetPool() const
    {
        return mPool;
    }

private:
    LogicalDevice               *mDevice = nullptr;
    vector<VkDescriptorPoolSize> mSizes;
    uint32_t                     mMaxSets;
    VkDescriptorPool             mPool = VK_NULL_HANDLE;
    dict_map<uint64_t, uint32_t> mSets;
};

}
} // namespace solis::graphics
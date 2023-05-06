#include "core/graphics/pipeline/descriptor_pool.hpp"
#include "core/graphics/logical_device.hpp"

namespace solis {
namespace graphics {
DescriptorPool::DescriptorPool(const vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets) :
    mSizes(sizes), mMaxSets(maxSets)
{
    mDevice = Graphics::Get()->GetLogicalDevice();

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount              = static_cast<uint32_t>(mSizes.size());
    poolInfo.pPoolSizes                 = mSizes.data();
    poolInfo.maxSets                    = mMaxSets;

    if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(mDevice, mPool, nullptr);
};

VkDescriptorSet DescriptorPool::Allocate(const VkDescriptorSetLayout &layout)
{
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool              = mPool;
    allocInfo.descriptorSetCount          = 1;
    allocInfo.pSetLayouts                 = &layout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(mDevice, &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    return descriptorSet;
}

}
} // namespace solis::graphics

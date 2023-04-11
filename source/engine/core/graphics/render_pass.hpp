#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/i_destroyable.hpp"

#include "volk.h"

namespace solis {
namespace graphics {

// 默认RenderPass实现
class SOLIS_CORE_API RenderPass : public Object<RenderPass>, public IDestroyable
{
public:
    RenderPass();
    virtual ~RenderPass();

    virtual void Destroy() override;

    // 这里就先用最简单的默认实现
    virtual bool Build();

    VkSubpassDescription &AddSubpass()
    {
        mSubpasses.emplace_back();
        return mSubpasses.back();
    }

    void AddSubpass(const VkSubpassDescription &subpass)
    {
        mSubpasses.push_back(subpass);
    }

    VkAttachmentDescription &AddAttachment()
    {
        mAttachments.emplace_back();
        return mAttachments.back();
    }

    void AddAttachment(const VkAttachmentDescription &attachment)
    {
        mAttachments.push_back(attachment);
    }

    VkSubpassDependency &AddDependency()
    {
        mDependencies.emplace_back();
        return mDependencies.back();
    }

    void AddDependency(const VkSubpassDependency &dependency)
    {
        mDependencies.push_back(dependency);
    }

    operator const VkRenderPass &() const
    {
        return mRenderPass;
    }

    virtual const VkRenderPass &GetRenderPass() const
    {
        return mRenderPass;
    }

private:
    VkRenderPass                    mRenderPass = VK_NULL_HANDLE;
    vector<VkAttachmentDescription> mAttachments;
    vector<VkSubpassDescription>    mSubpasses;
    vector<VkSubpassDependency>     mDependencies;
};
}
} // namespace solis::graphics

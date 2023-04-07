#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/i_destroyable.hpp"

#include "volk.h"

namespace solis {
namespace graphics {

class SOLIS_CORE_API Subpass : public Object<Subpass>
{
public:
    Subpass()          = default;
    virtual ~Subpass() = default;

    VkAttachmentReference AddColorAttachment()
    {
        mColorAttachments.emplace_back();
        return mColorAttachments.back();
    }

    void EnableDepthStencil(bool enable = true)
    {
        mEnableDepthStencil = enable;
    }

    const vector<VkAttachmentReference> &GetColorAttachments() const
    {
        return mColorAttachments;
    }

    VkAttachmentReference &GetDepthStencilAttachment()
    {
        return mDepthStencilAttachment;
    }

    VkSubpassDescription GetDescription() const
    {
        VkSubpassDescription description = {};
        description.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        description.colorAttachmentCount = static_cast<uint32_t>(mColorAttachments.size());
        description.pColorAttachments    = mColorAttachments.data();
        if (mEnableDepthStencil)
        {
            description.pDepthStencilAttachment = &mDepthStencilAttachment;
        }
        return description;
    }

private:
    bool                          mEnableDepthStencil = false;
    vector<VkAttachmentReference> mColorAttachments;
    VkAttachmentReference         mDepthStencilAttachment;
};

// 默认RenderPass实现
class SOLIS_CORE_API RenderPass : public Object<RenderPass>, public IDestroyable
{
public:
    RenderPass();
    virtual ~RenderPass();

    virtual void Destroy() override;

    // 这里就先用最简单的默认实现
    virtual bool Build();

    Subpass &AddSubpass()
    {
        mSubpasses.emplace_back();
        return mSubpasses.back();
    }

    VkAttachmentDescription &AddAttachment()
    {
        mAttachments.emplace_back();
        return mAttachments.back();
    }

    VkSubpassDependency &AddDependency()
    {
        mDependencies.emplace_back();
        return mDependencies.back();
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
    vector<Subpass>                 mSubpasses;
    vector<VkSubpassDependency>     mDependencies;
};
}
} // namespace solis::graphics

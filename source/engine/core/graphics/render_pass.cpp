#include "core/graphics/graphics.hpp"
#include "core/graphics/logical_device.hpp"

#include "core/graphics/render_pass.hpp"

#include "core/log/log.hpp"

namespace solis {
namespace graphics {

RenderPass::RenderPass()
{
    Graphics::Get()->AddRenderPass(this);
}

RenderPass::~RenderPass()
{
    Destroy();
};

void RenderPass::Destroy()
{
    if (IsDestroyed())
    {
        return;
    }

    if (mRenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(*Graphics::Get()->GetLogicalDevice(), mRenderPass, nullptr);
        mRenderPass = VK_NULL_HANDLE;
    }

    mDestroyed = true;

    // TODO: 从Graphics中移除
}

bool RenderPass::Build()
{
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = mAttachments.size();
    renderPassInfo.pAttachments    = mAttachments.data();
    renderPassInfo.subpassCount    = mSubpasses.size();
    renderPassInfo.pSubpasses      = mSubpasses.data();
    renderPassInfo.dependencyCount = mDependencies.size();
    renderPassInfo.pDependencies   = mDependencies.data();

    if (vkCreateRenderPass(*Graphics::Get()->GetLogicalDevice(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
    {
        Log::SError("failed to create render pass!");
        return false;
    }

    return true;
}

}
} // namespace solis::graphics
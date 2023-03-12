#include "core/graphics/graphics.hpp"
#include "core/graphics/pipeline/pipeline.hpp"

namespace solis {
namespace graphics {

Pipeline::Pipeline()
{
    Graphics::Get()->AddPipeline(this);
}

Pipeline::~Pipeline()
{
    Destroy();
}

void Pipeline::Destroy()
{
    for (auto &pair : mUnionBufferMap)
    {
        for (auto &buffer : pair.second)
        {
            buffer->Destroy();
        }
    }
    mUnionBufferMap.clear();
}

void Pipeline::InitUniformBuffers(const Swapchain *swapchain, size_t size, size_t count)
{
    auto it = mUnionBufferMap.find(size_t(uintptr_t(swapchain)));

    if (it == mUnionBufferMap.end())
    {
        auto pair = mUnionBufferMap.insert({size_t(uintptr_t(swapchain)), {}});
        if (pair.second)
        {
            it = pair.first;
        }

        auto &unionBuffers = it->second;
        unionBuffers.clear();
        for (size_t i = 0; i < count; ++i)
        {
            unionBuffers.emplace_back(std::make_shared<Buffer>(Buffer::Type::Uniform, size));
        }
    }
}

Buffer &Pipeline::GetUniformBuffer(const Swapchain *swapchain, size_t index)
{
    auto it = mUnionBufferMap.find(size_t(uintptr_t(swapchain)));

    if (it == mUnionBufferMap.end())
    {
        return *(mUnionBufferMap.begin()->second[0]);
    }

    return *(it->second[index]);
}
}
} // namespace solis::graphics
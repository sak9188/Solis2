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
    for (auto &buffer : mUnionBuffers)
    {
        buffer->Destroy();
    }
    mUnionBuffers.clear();
}

void Pipeline::InitUniformBuffers(size_t size)
{
    // TODO: Get from Graphics
    for (int i = 0; i < MaxFrameInFlight; ++i)
    {
        mUnionBuffers.emplace_back(std::make_shared<Buffer>(Buffer::Type::Uniform, size));
    }
}

Buffer &Pipeline::GetUniformBuffer(size_t index)
{
    return *mUnionBuffers[index];
}
}
} // namespace solis::graphics
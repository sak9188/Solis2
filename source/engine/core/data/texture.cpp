#include "core/data/texture.hpp"
#include "core/log/log.hpp"

#include "core/graphics/buffer/buffer.hpp"

#include "stb/stb_image.h"

namespace solis {
Texture::Texture(const string &path)
{
    using Buffer = graphics::Buffer;
    using Image  = graphics::Image;

    int      width, height, nrChannels;
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data)
    {
        VkDeviceSize imageSize = width * height * 4;
        Buffer       buffer(Buffer::Type::Stage, imageSize, data);

        // 此时已经将数据从CPU复制到了GPU的内存中
        stbi_image_free(data);

        // 创建GpuImage
        VkExtent2D extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        mImage = std::make_unique<Image>(extent,
                                         VK_FORMAT_R8G8B8A8_SRGB,
                                         VK_SAMPLE_COUNT_1_BIT,
                                         VK_IMAGE_TILING_OPTIMAL,
                                         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                         VK_FILTER_LINEAR);

        mImage->TransitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        mImage->Update(buffer);
        mImage->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    else
    {
        stbi_image_free(data);
        Log::SError("Failed to load texture {}", path);
        // throw std::runtime_error("Failed to load texture");
    }
}
} // namespace solis

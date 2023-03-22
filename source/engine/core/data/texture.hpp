#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/graphics/image/image.hpp"

namespace solis {

class SOLIS_CORE_API Texture : public Object<Texture>
{
public:
    Texture(const string &path);
    ~Texture() = default;

    const graphics::Image &GetImage() const
    {
        return *mImage.get();
    }

private:
    std::unique_ptr<graphics::Image> mImage;
};
} // namespace solis
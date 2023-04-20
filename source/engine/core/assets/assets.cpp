#include "core/assets/assets.hpp"
#include "core/assets/asset_class.hpp"

#include "core/files/files.hpp"

namespace solis {
namespace assets {

std::unique_ptr<Asset> Assets::Load(const string &path)
{
    auto asset   = std::make_unique<Asset>();
    asset->mType = type;
    asset->mData = IAset::Create(type);
    asset->mPath = path;
    return asset;
}

std::unique_ptr<Asset> Assets::AsyncLoad(const string &path)
{
    auto asset     = std::make_unique<Asset>();
    asset->mPath   = path;
    asset->mLoaded = true;
    return asset;
}
}

} // namespace solis::assets
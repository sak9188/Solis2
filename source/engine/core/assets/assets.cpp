#include "core/assets/assets.hpp"
#include "core/assets/asset_class.hpp"

#include "core/files/files.hpp"

namespace solis {
namespace assets {

std::shared_ptr<Asset> Assets::Load(const string &path)
{
    if (mAssets.find(path) != mAssets.end())
    {
        auto asset = mAssets[path];
        if (asset.lock())
        {
            return asset.lock();
        }
    }

    files::FileInfo fi{path};
    if (!fi.Exist())
    {
        return nullptr;
    }

    auto asset = std::make_shared<FileAsset>(fi.ReadBytes(), path);
    return asset;
}

std::shared_ptr<Asset> Assets::AsyncLoad(const string &path)
{
    // auto asset     = std::make_unique<Asset>();
    // asset->mPath   = path;
    // asset->mLoaded = true;
    // return asset;
    return nullptr;
}

bool Assets::OnAssetReleased(const AssetReleasedEvent &event)
{
    auto asset = event.asset;
    if (mAssetNames.find(asset) != mAssetNames.end())
    {
        mReleasedAssets.insert(mAssetNames[asset].begin(), mAssetNames[asset].end());
        mAssetNames.erase(asset);
    }
    return true;
}
}
} // namespace solis::assets
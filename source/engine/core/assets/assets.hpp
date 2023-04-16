
#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace assets {

/**
 * @brief 手搓的资源不要放在Assets里面，放在自己的模块里面
 * 这里的资源主要是针对类(继承了IAsset)的资源,比如Mesh,Texture,Material等
 *
 */
class SOLIS_CORE_API Assets : public Object<Assets>, public Module::Registrar<Assets>
{
    inline static const bool Registered = Register(Stage::Pre, Requires<files::Files>);

public:
    Assets()          = default;
    virtual ~Assets() = default;

    virtual void Update() override
    {
    }

    /**
     * @brief 同步资源加载
     *
     * @param path
     * @return std::unique_ptr<Asset>
     */
    std::unique_ptr<Asset> Load(const string &path)
    {
        auto asset     = std::make_unique<Asset>();
        asset->mData   = LoadFile(path);
        asset->mLoaded = true;
        return asset;
    }

    /**
     * @brief 异步加载资源，返回一个空的Asset，当资源加载完成后，会将资源数据填充到Asset中
     *
     * @param path
     * @return std::unique_ptr<Asset>
     */
    std::unique_ptr<Asset> AsyncLoad(const string &path)
    {
        auto asset     = std::make_unique<Asset>();
        asset->mData   = LoadFile(path);
        asset->mLoaded = true;
        return asset;
    }
};
}
} // namespace solis::assets

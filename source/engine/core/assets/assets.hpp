
#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/events/event_define.hpp"

#include "core/assets/asset_class.hpp"

#include <set>

namespace solis {

namespace files {
class Files;
}

namespace assets {

/**
 * @brief 手搓的资源不要放在Assets里面，放在自己的模块里面
 * 这里的资源主要是针对类(继承了IAsset)的资源,比如Mesh,Texture,Material等
 * 23/4/22：资产系统可能需要大改，主要是为了解决资源依赖问题
 */
class SOLIS_CORE_API Assets : public Object<Assets>, public Module::Registrar<Assets>, public EventHandler
{
    inline static const bool Registered = Register(Stage::Pre, Requires<files::Files>());

public:
    OBJECT_NEW_DELETE(Assets);

    Assets()          = default;
    virtual ~Assets() = default;

    virtual void Update() override
    {
    }

    /**
     * @brief Get the Asset object
     *
     * @param pathORName
     * @return std::shared_ptr<Asset>
     */
    std::shared_ptr<Asset> GetAsset(const string &pathORName)
    {
        if (pathORName.empty())
        {
            return nullptr;
        }
        if (mAssets.find(pathORName) != mAssets.end())
        {
            return mAssets[pathORName].lock();
        }
        if (mAssetAliases.find(pathORName) != mAssetAliases.end())
        {
            return mAssetAliases[pathORName].lock();
        }
        return nullptr;
    }

    /**
     * @brief 强引用这个资源
     *
     * @param pathORName
     */
    bool RetainAsset(const string &pathORName)
    {
        return RetainAsset(GetAsset(pathORName));
    }

    /**
     * @brief 强引用这个资源
     *
     * @param pathORName
     */
    bool RetainAsset(const std::shared_ptr<Asset> &asset)
    {
        if (!asset)
        {
            return false;
        }
        mAssetRefs.insert(asset);
        return true;
    }

    /**
     * @brief 释放这个资源
     *
     * @param pathORName
     */
    void ReleaseAsset(const string &pathORName)
    {
        ReleaseAsset(GetAsset(pathORName));
    }

    /**
     * @brief 释放这个资源
     *
     * @param asset
     */
    void ReleaseAsset(const std::shared_ptr<Asset> &asset)
    {
        if (!asset)
        {
            return;
        }
        mAssetRefs.erase(asset);
    }

    /**
     * @brief 针对文件资源的别名
     *
     * @param alias
     * @param path
     */
    void AliasAsset(const string &alias, const string &path);

    /**
     * @brief 针对类资源的别名，比如Mesh,Texture,Material等
     *
     * @param alias
     * @param asset
     */
    void AliasAsset(const string &alias, const std::shared_ptr<Asset> &asset);

    /**
     * @brief 载入文件资源，非从类序列化好的资源
     *
     * @tparam T
     * @param path
     * @return std::unique_ptr<T>
     */
    template <typename T, typename... Args>
    std::shared_ptr<Asset> LoadObject(Args &&...args)
    {
        return std::shared_ptr<ClassAsset<T>>(std::forward<Args>(args)...);
    }

    /**
     * @brief 异步载入类资源
     *
     * @tparam T
     * @tparam Args
     * @param args
     * @return std::shared_ptr<Asset>
     */
    template <typename T, typename... Args>
    std::shared_ptr<Asset> AsyncLoadObject(Args &&...args)
    {
        // TODO: 这里需要结合线程组来写
        throw std::runtime_error("not implemented");
    }

    /**
     * @brief 同步资源加载(主要是载入序列化的类资源)
     *
     * @param path
     * @return std::unique_ptr<Asset>
     */
    std::shared_ptr<Asset> Load(const string &path);

    /**
     * @brief 异步加载资源，返回一个空的Asset，当资源加载完成后，会将资源数据填充到Asset中
     *
     * @param path
     * @return std::unique_ptr<Asset>
     */
    std::shared_ptr<Asset> AsyncLoad(const string &path);

    /**
     * @brief 释放资源时会主动调用这个函数
     *
     * @param event
     * @return true
     * @return false
     */
    bool OnAssetReleased(const AssetReleasedEvent &event);

private:
    // 这里不存强引用，如果不保存就直接释放，如果在某段时间内可能存在反复释放的情况
    // 那么久AddRef的方式保存这个引用, 在确定不会再使用的时候，再Release
    // 红黑树，方便查找
    // TODO: 可以优化成LRU
    sort_map<string, std::weak_ptr<Asset>> mAssets;
    sort_map<string, std::weak_ptr<Asset>> mAssetAliases;

    /**
     * @brief 反向索引
     */
    dict_map<Asset *, std::set<string_view>> mAssetNames;

    // 这里存储所有的Asset的强引用，防止Asset被释放
    std::set<std::shared_ptr<Asset>> mAssetRefs;

    // 这里记录了mAssets中资源被释放的个数
    // 如果超过了一半，就会在下一帧清理一
    // TODO :
    std::set<string_view> mReleasedAssets;
};
} // namespace assets
} // namespace solis

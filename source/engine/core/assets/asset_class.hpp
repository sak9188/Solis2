#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace assets {

class IAsset
{
public:
    virtual ~IAsset() = default;

    struct AssetFunction
    {
        bool (*assertlize)(void *object);
        bool (*deassertlize)(const vector<uint8_t> &data);
    };

    template <typename T, vector<uint8_t> (T::*Assetlize)(), bool (T::*DeAssetlize)(const vector<uint8_t> &data)>
    inline static const bool RegisterAsset()
    {
        static dict_map<uint64_t, AssetFunction> AssetsFunctionMap;

        uint64_t      hash = typeid(T).hash_code();
        AssetFunction func;
        func.assertlize         = (bool (*)(void *))Assetlize;
        func.deassertlize       = (bool (*)(const vector<uint8_t> &))DeAssetlize;
        AssetsFunctionMap[hash] = func;
    }
};

/**
 * @brief 资源接口, 大部分的类需要实现这个接口
 *
 */
template <typename T, vector<uint8_t> (T::*Assetlize)(), bool (T::*DeAssetlize)(const vector<uint8_t> &data)>
class SOLIS_CORE_API AssetBase : public IAsset
{
    inline static const bool Registered = RegisteAsset<T>(Assetlize, DeAssetlize);

public:
    /**
     * @brief 将资源序列化为二进制数据
     *
     * @return vector<uint8_t>
     */
    vector<uint8_t>
    Assetlize() = 0;

    /**
     * @brief  将二进制数据反序列化为资源
     *
     * @param data
     * @return true
     * @return false
     */
    bool DeAssetlize(const vector<uint8_t> &data) = 0;
};

class SOLIS_CORE_API Asset : public Object<Asset>
{
    friend class Assets;

public:
    virtual ~Asset() = default;

    // IsLoaded
    bool IsLoaded() const
    {
        return mLoaded;
    }

    // Save
    bool Save(const string &path)
    {
        return false;
    }

    // AsyncSave
    bool AsyncSave(const string &path)
    {
        return false;
    }

    /**
     * @brief  等待资源加载完成
     *
     * @return true
     * @return false
     */
    bool WaitLoad()
    {
        return false;
    }

    template <typename T>
    T *Data()
    {
        return static_cast<T *>(mData);
    }

    /**
     * @brief 计算资源的Hash值，用于资源的比较
     *
     * @return uint64_t
     */
    uint64_t Hash()
    {
        return 0;
    }

protected:
    Asset() = default;

private:
    uint64_t mType   = 0;       // 资源类型
    IAsset  *mData   = nullptr; // 资源数据
    bool     mLoaded = false;   // 是否已经加载
    uint64_t mHash   = 0;       // 缓存Hash值
};
}
} // namespace solis::assets
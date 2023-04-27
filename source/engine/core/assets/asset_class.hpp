#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/files/file_info.hpp"
#include "core/events/event_property.hpp"

namespace solis {

namespace assets {
class Asset;
}

struct AssetReleasedEvent : public Event
{
    AssetReleasedEvent(assets::Asset *asset) :
        asset(asset)
    {
    }
    assets::Asset *asset = nullptr;
};

namespace assets {

class IAsset
{
public:
    virtual ~IAsset() = default;

    struct AssetFunction
    {
        void *(*newobject)(void);
        bool (*assertlize)(void *object);
        bool (*deassertlize)(void *object, const vector<uint8_t> &data);
    };

    template <typename T, vector<uint8_t> (T::*Assetlize)(), bool (T::*DeAssetlize)(const vector<uint8_t> &data)>
    static const bool RegisterAsset()
    {
        uint64_t      hash = typeid(T).hash_code();
        AssetFunction func;
        func.newobject = (void *(*)())[]()->T *
        {
            // 这个Assert类型必定是POD类型
            return new T();
        };
        func.assertlize         = (bool (*)(void *))Assetlize;
        func.deassertlize       = (bool (*)(void *, const vector<uint8_t> &))DeAssetlize;
        AssetsFunctionMap[hash] = func;
    }

    static IAsset *Create(const string &path, uint64_t &type)
    {
        using namespace files;
        FileInfo fi{path};
        if (!fi.Exist())
        {
            return nullptr;
        }

        vector<uint8_t> data = fi.ReadBytes();
        // TODO: 这里有可能有大小端的影响
        // 读前8个字节, 作为类型
        type          = *reinterpret_cast<uint64_t *>(data.data());
        IAsset *asset = reinterpret_cast<IAsset *>(AssetsFunctionMap[type].newobject());

        // 读后面的字节, 作为数据
        vector<uint8_t> assetData(data.data() + 8, data.data() + data.size());
        AssetsFunctionMap[type].deassertlize(asset, assetData);

        // 这里不对释放内存进行负责
        return asset;
    }

private:
    inline static dict_map<uint64_t, AssetFunction> AssetsFunctionMap;
};

/**
 * @brief 资源接口, 大部分的类需要实现这个接口
 *
 */
template <typename T, vector<uint8_t> (T::*Assetlize)(), bool (T::*DeAssetlize)(const vector<uint8_t> &data)>
class SOLIS_CORE_API ClassAssetBase : public IAsset
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
    virtual ~Asset()
    {
        OnReleased.Invoke({this});
    }

    virtual bool IsClassAsset()
    {
        return false;
    }

    virtual bool IsFileAsset()
    {
        return false;
    }

    /**
     * @brief 对于文件资源，刷新资源，对于内存资源，不需要刷新
     *
     * @return true
     * @return false 如果是假，那么说明资源刷新失败，需要卸载这个资源
     */
    virtual bool Refresh()
    {
        return true;
    };

    /**
     * @brief 这里不做类型安全检查
     *
     * @tparam T
     * @return void*
     */
    template <typename T>
    T *Data()
    {
        return static_cast<T *>(Data());
    }

    virtual void *Data() = 0;

    /**
     * @brief 使用此方法，相当于把资源移动到了外部，资源管理器不再管理这个资源
     *
     * @return std::shared_ptr<void *>
     */
    virtual std::shared_ptr<void> FetchData() = 0;

    /**
     * @brief 计算资源的Hash值，用于资源的比较
     *
     * @return uint64_t
     */
    uint64_t Hash()
    {
        return 0;
    }

    EventProperty<AssetReleasedEvent> OnReleased;

protected:
    Asset() = default;

    string   mName = ""; // 资源名字()
    uint64_t mHash = 0;  // 缓存Hash值
};

template <typename T>
class ClassAsset : public Asset
{
    // 这里暂时先不这样设计
    // static_assert(std::is_base_of_v<ClassAssetBase, T>, "T must be derived from IAsset");

public:
    template <typename... Args>
    ClassAsset(Args &&...args)
    {
        mData = std::make_unique<T>(std::forward<Args>(args)...);
    }

    virtual bool IsClassAsset() override final
    {
        return true;
    }

    template <typename T>
    T &Data()
    {
        assert(mData != nullptr);
        assert(mType == ctti::type_id<T>());
        return *mData.get();
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
     * @brief 没有类型检查
     *
     * @return void*
     */
    virtual void *Data() override
    {
        return static_cast<void *>(mData.get());
    }

    virtual std::shared_ptr<void> FetchData() override
    {
        auto ret = std::make_shared<T>(std::move(*mData.get()));
        mData.reset();
        return ret;
    }

private:
    ctti::type_id_t    mType = ctti::type_id<T>(); // 资源类型
    std::unique_ptr<T> mData = nullptr;            // 资源数据
};

class SOLIS_CORE_API FileAsset : public Asset
{
public:
    virtual bool IsFileAsset() override final
    {
        return true;
    }

    /**
     * @brief 默认构造适用于异步加载的资源
     *
     */
    FileAsset() = default;

    /**
     * @brief Construct a new File Asset object
     *
     * @param data
     * @param path
     */
    FileAsset(const vector<uint8_t> &data, const string &path)
    {
    }

    virtual void *Data() override
    {
        return mData.data();
    }

    virtual std::shared_ptr<void> FetchData() override
    {
        auto ret = std::make_shared<vector<uint8_t>>(std::move(mData));
        mData.clear();
        return ret;
    }

    size_t GetDataSize() const noexcept
    {
        return mData.size();
    }

    vector<uint8_t> DataBytes() const noexcept
    {
        return mData;
    }

    // IsLoaded
    bool IsLoaded() const
    {
        return mData.size() > 0;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool WaitLoad()
    {
        return false;
    }

private:
    vector<uint8_t> mData{};    // 资源数据
    string          mPath = ""; // 资源路径
};

} // namespace assets
} // namespace solis
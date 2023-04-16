#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace assets {
class SOLIS_CORE_API IAsset
{
};

class SOLIS_CORE_API Asset : public Object<Asset>, public IAsset
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
    void    *mData   = nullptr; // 资源数据
    bool     mLoaded = false;   // 是否已经加载
    uint64_t mHash   = 0;       // 缓存Hash值
};
}
} // namespace solis::assets
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/i_destroyable.hpp"

#include "core/data/texture.hpp"
#include "core/graphics/pipeline/pipeline.hpp"

#include "volk.h"

namespace solis::graphics {
class SOLIS_CORE_API Material : public Object<Material>, public IDestroyable
{
public:
    /**
     * @brief Construct a new Material object
     *
     * @param shaderDirPath
     */
    Material(const string &shaderDirPath, const string &passName = "");

    /**
     * @brief Construct a new Material object
     *
     * @param vs
     * @param ps
     */
    Material(const string &vs, const string &ps, const string &passName = "");

    virtual ~Material()
    {
        Destroy();
    };

    virtual void Destroy() override
    {
        if (mDestroyed)
            return;

        mPipeline->Destroy();
        mDestroyed = true;
    }

    /**
     * @brief Set the Pass Node object
     * 如果默认是空，则默认走Graph提供好的主要光照Pass
     * 如果不为空，则走自定义的Pass
     * 且下一帧发生改变
     * @param nodeName
     */
    void SetPassNode(string nodeName)
    {
    }

    template <typename T>
    void SetUniform(const string &name, const T &value)
    {
    }

    void SetSampler(const string &name, const Texture *sampler)
    {
    }

private:
    string mPassNodeName = "";
    // 目前不存在Compute管线
    std::unique_ptr<Pipeline> mPipeline = nullptr;
};
} // namespace solis::graphics
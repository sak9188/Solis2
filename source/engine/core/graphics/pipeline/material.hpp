#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/i_destroyable.hpp"

#include "core/data/texture.hpp"

#include "volk.h"

namespace solis::graphics {
class SOLIS_CORE_API Material : public Object<Material>, public IDestroyable
{
public:
    Material(const string &shaderDirPath){};
    Material(const string &vs, const string &ps){};
    virtual ~Material() = default;

    /**
     * @brief Set the Pass Node object
     * 如果默认是空，则默认走Graph提供好的主要光照Pass
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
};
} // namespace solis::graphics
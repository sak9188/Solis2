#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "volk.h"

namespace solis::graphics {
class SOLIS_CORE_API Material : public Object<Material>, public IDestroyable
{
public:
    Material(const string &vs, const string &ps) = default;
    virtual ~Material()                          = default;

    // 设置所依赖的PassNode
    void SetPassNode(string nodeName)
    {
    }

    template <typename T>
    void SetUniform(const string &name, const T &value)
    {
    }

private:
    string mPassNodeName;
};
} // namespace solis::graphics
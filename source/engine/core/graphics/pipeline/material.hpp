#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/i_destroyable.hpp"

#include "volk.h"

namespace solis::graphics {
class SOLIS_CORE_API Material : public Object<Material>, public IDestroyable
{
public:
    Material(const string &vs, const string &ps){};
    virtual ~Material() = default;

    /**
     * @brief Set the Pass Node object
     *
     * @param nodeName
     */
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
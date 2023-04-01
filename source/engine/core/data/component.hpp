#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

namespace solis {
struct SOLIS_CORE_API Component : public Object<Component>
{
public:
    Component()  = default;
    ~Component() = default;
};

using ComponentMap = dict_map<uint64_t, Component *>;
} // namespace solis

#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

namespace solis {
namespace assets {
class SOLIS_CORE_API Assets : public Object<Assets>, public Module::Registrar<Assets>
{
    inline static const bool Registered = Register(Stage::Pre, Requires<files::Files>);

public:
    Assets()          = default;
    virtual ~Assets() = default;

    virtual void Update() override
    {
    }
};
}
} // namespace solis::assets

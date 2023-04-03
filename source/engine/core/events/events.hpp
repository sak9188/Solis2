
#pragma once

#include "core/solis_core.hpp"

#include "core/base/object.hpp"
#include "core/base/using.hpp"
#include "core/base/module.hpp"

#include "core/events/event_manager.hpp"

#include <filesystem>
#include <iostream>

namespace solis {
namespace events {

class SOLIS_CORE_API Events : public Object<Events>, public EventManager, public Module::Registrar<Events>
{
    inline static const bool Registered = Register(Stage::Never);

public:
    Events()          = default;
    virtual ~Events() = default;

    virtual void Update() override
    {
    }
};
}
} // namespace solis::events
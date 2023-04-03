
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

namespace solis {
namespace world {
class SOLIS_CORE_API World : public Object<World>, public Module::Registrar<World>
{
    inline static const bool Registered = Register(Stage::Normal);

public:
    World()          = default;
    virtual ~World() = default;

    virtual void Update() override{};
};
}
} // namespace solis::world
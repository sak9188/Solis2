
#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

namespace solis {
class SOLIS_CORE_API WorldBase : public Object<WorldBase>
{
public:
    WorldBase()          = default;
    virtual ~WorldBase() = default;

    virtual void Start() override{};
    virtual void Update() override{};
};
} // namespace solis
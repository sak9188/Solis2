#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

namespace solis {
namespace components {
struct SOLIS_CORE_API Transform : public Component
{
    math::vec3 mPosition;
    math::vec3 mRotation;
    math::vec3 mScale;
};
}
} // namespace solis::components
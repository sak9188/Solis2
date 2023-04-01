#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

namespace solis {
struct SOLIS_CORE_API TransformComponent : public Component
{
    math::vec3 mPosition;
    math::vec3 mRotation;
    math::vec3 mScale;
};
} // namespace solis
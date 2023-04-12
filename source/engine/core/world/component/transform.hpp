#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

namespace solis {
namespace components {
struct SOLIS_CORE_API Transform : public Component<Transform>
{
    math::vec3 position{0.0f, 0.0f, 0.0f};
    math::vec3 rotation{0.0f, 0.0f, 0.0f};
    math::vec3 scale{1.0f, 1.0f, 1.0f};
};
}
} // namespace solis::components

#pragma once

#include "core/solis_core.hpp"
#include "core/base/object.hpp"
#include "core/base/using.hpp"

#include "core/data/component.hpp"

#include "core/world/base/i_renderable.hpp"

namespace solis {
namespace components {
class SOLIS_CORE_API Renderable : public Component<Renderable>
{
};
}
} // namespace solis::components
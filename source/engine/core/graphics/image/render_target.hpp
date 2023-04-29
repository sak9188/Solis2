#pragma once

#include "core/solis_core.hpp"
#include "core/base/using.hpp"
#include "core/base/object.hpp"

#include "volk.h"
#include "vk_mem_alloc.h"

namespace solis {
namespace graphics {

// this is only a shell
class SOLIS_CORE_API RenderTarget : public Object<RenderTarget>
{
public:
    RenderTarget();
    virtual ~RenderTarget();
}
}
} // namespace solis::graphics

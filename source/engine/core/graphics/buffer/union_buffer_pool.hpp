#pragma once

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/i_destroyable.hpp"

#include "volk.h"
#include "vk_mem_alloc.h"

namespace solis {
namespace graphics {
class SOLIS_CORE_API UnionBufferPool : public Object<UnionBufferPool>, public IDestroyable
{
};
}
} // namespace solis::graphics
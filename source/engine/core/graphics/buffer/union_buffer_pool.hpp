#pragma once

#include "volk.h"

#include "core/base/using.hpp"
#include "core/base/object.hpp"
#include "core/base/i_destroyable.hpp"
#include "core/graphics/vma.hpp"

namespace solis {
namespace graphics {

/**
 * @brief 这个类主要是一个Shell
 */
class SOLIS_CORE_API UinonBuffer : public Object<UinonBuffer>, public IDestroyable
{
};

/**
 * @brief 主要是分配和管理UnionBuffer
 */
class SOLIS_CORE_API UnionBufferPool : public Object<UnionBufferPool>,
                                       public IDestroyable
{
};
}
} // namespace solis::graphics
#include "core/base/memory.hpp"
#include "mimalloc-2.0/mimalloc.h"

namespace solis {
void MemoryDebuger::StatusMemory()
{
    mi_stats_print(nullptr);
}

void MemoryDebuger::ResetStatusMemory()
{
    mi_stats_reset();
}
} // namespace solis
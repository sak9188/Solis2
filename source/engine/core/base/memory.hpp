#pragma once

#include "core/solis_core.hpp"

namespace solis {
class SOLIS_CORE_API MemoryDebuger
{
public:
    MemoryDebuger()
    {
    }
    virtual ~MemoryDebuger() = default;

    void DumpMemoryLeak()
    {
        _CrtDumpMemoryLeaks();
    }

    static void StatusMemory();
    static void ResetStatusMemory();
};
} // namespace solis

#pragma once

#include "core/solis_core.hpp"

namespace solis {
class SOLIS_CORE_API MemoryDebuger
{
public:
    MemoryDebuger()
    {
        _CrtSetReportMode(_CRT_WARN | _CRT_ERROR, _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_WARN | _CRT_ERROR, _CRTDBG_FILE_STDOUT);
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

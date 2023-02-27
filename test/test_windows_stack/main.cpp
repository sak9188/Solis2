#include <windows.h>
#include <cstdio>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

typedef USHORT(WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID *, __out_opt PULONG);
CaptureStackBackTraceType func = (CaptureStackBackTraceType)(GetProcAddress(LoadLibrary("kernel32.dll"), "RtlCaptureStackBackTrace"));

void Stack1();

void PrintCallStack()
{
    if (func == NULL)
        return; // WOE 29.SEP.2010

    // Quote from Microsoft Documentation:
    // ## Windows Server 2003 and Windows XP:
    // ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
    const int kMaxCallers = 62;

    void *callers[kMaxCallers];
    int count = (func)(0, kMaxCallers, callers, NULL);

    auto process = GetCurrentProcess();
    auto processSym = SymInitialize(process, NULL, TRUE);

    for (int i = 0; i < count; ++i)
    {
        DWORD64 address = (DWORD64)(callers[i]);
        DWORD64 displacement = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        if (SymFromAddr(process, address, &displacement, symbol))
        {
            printf("%i: %s - 0x%0X\n", i, symbol->Name, symbol->Address);
        }
    }
}

void Stack1()
{
    PrintCallStack();
}

void Stack2()
{
    Stack1();
}

void Stack3()
{
    Stack2();
}

int main()
{
    Stack3();
    return 0;
}

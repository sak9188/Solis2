#ifdef SOLIS_CORE_LIB
    #define SOLIS_CORE_API __declspec(dllexport)
#else
    #define SOLIS_CORE_API __declspec(dllimport)
#endif
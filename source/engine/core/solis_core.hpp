#pragma once

#if defined(SOLIS_CORE_LIB)
#if defined(_MSC_VER)
#if defined(USE_SOLIS_CORE_LIB)
#define SOLIS_CORE_API __declspec(dllexport)
#else
#define SOLIS_CORE_API __declspec(dllimport)
#endif
#elif defined(__clang__) || defined(__GNUC__)
#if defined(USE_SOLIS_CORE_LIB)
#define SOLIS_CORE_API __attribute__((__visibility__("default")))
#else
#define SOLIS_CORE_API __attribute__((__visibility__("default")))
#endif
#else
#define SOLIS_CORE_API
#endif
#else
#define SOLIS_CORE_API
#endif

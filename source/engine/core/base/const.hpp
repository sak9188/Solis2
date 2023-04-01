#pragma once

#include "core/solis_core.hpp"

namespace solis {
inline const uint32_t EngineVersionMajor = 0;

inline const uint32_t EngineVersionMinor = 1;

inline const uint32_t EngineVersionPatch = 0;

inline const char *EngineName = "Solis";

// Memory
// 最大 32 个模块
inline const size_t MaxModules = 32;

// 最大 1000 堆上的对象
inline const size_t GlobalObjectSize = 1000;

// ECS
// PoolNode初始大小
inline const size_t ObjectPoolInitSize = 32;
// PoolNode扩容倍数
inline const size_t ObjectPoolStepTimesSize = 2;

// 最大VertexAttribute数量
inline const size_t MaxVertexAttributes = 16;

// 最大FrameInFlight数量
inline const size_t MaxFrameInFlight = 3;

// Max Swapchain
inline const size_t MaxSwapchain = 5;
} // namespace solis
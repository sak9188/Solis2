#pragma once

#include "core/solis_core.hpp"

namespace solis {
inline const uint32_t EngineVersionMajor = 0;

inline const uint32_t EngineVersionMinor = 1;

inline const uint32_t EngineVersionPatch = 0;

inline const char *EngineName = "Solis";

// 最大 32 个模块
inline const size_t MaxModules = 32;

// 最大 1000 堆上的对象
inline const size_t GlobalObjectSize = 1000;

// 最大VertexAttribute数量
inline const size_t MaxVertexAttributes = 16;
} // namespace solis
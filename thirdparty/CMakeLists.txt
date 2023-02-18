# GLM
add_subdirectory(glm)

# glfw
add_subdirectory(glfw)

# ASTC

# GLSLANG
add_subdirectory(glslang)

# mimalloc
add_subdirectory(mimalloc)

# physfs
add_subdirectory(physfs)

# IMGUI

# SPDLOG
add_subdirectory(spdlog)

# STB
add_subdirectory(stb)

# VOLK
add_subdirectory(volk)

# VULKAN
find_package(Vulkan REQUIRED)

# VMA
add_subdirectory(VulkanMemoryAllocator)

# Boost
# add_subdirectory(boost_1_78)

# fmt
set(FMT_INSTALL ON)
add_subdirectory(fmt)

# Folly
# add_subdirectory(folly_2023_02_13)

# FreeType

# PyBind11

# TracyClient

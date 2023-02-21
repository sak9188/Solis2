if(${BUILD_THIRDPARTY})
    # CTTI
    set(CTTI_INSTALL ON)
    add_subdirectory(ctti)

    # GLM
    set(GLM_INSTALL ON)
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
    set(VOLK_INSTALL ON)
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
else()
    # ctti
    set(ctti_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/ctti CACHE INTERNAL "")
    find_package(ctti CONFIG REQUIRED)

    # mimalloc
    set(mimalloc_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/mimalloc-2.0 CACHE INTERNAL "")
    find_package(mimalloc CONFIG REQUIRED)

    # GLM
    set(glm_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/glm CACHE INTERNAL "")
    find_package(glm CONFIG REQUIRED)

    # VOLK
    set(volk_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/volk CACHE INTERNAL "")
    find_package(volk CONFIG REQUIRED)

    # glfw
    set(glfw3_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/glfw3 CACHE INTERNAL "")
    find_package(glfw3 CONFIG REQUIRED)

    # FMT
    set(fmt_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/fmt CACHE INTERNAL "")
    find_package(fmt CONFIG REQUIRED)

    # Boost
    set(BOOST_VERSION 1.78.0)
    set(Boost_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/Boost-${BOOST_VERSION} CACHE INTERNAL "")
    find_package(Boost CONFIG REQUIRED)

    # folly
    set(folly_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/folly CACHE INTERNAL "")
    find_package(folly CONFIG REQUIRED)
endif()
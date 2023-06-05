# =============================================================================
# CMake 配置
# =============================================================================
cmake_minimum_required(VERSION 3.20)

# 设置c++版本要求
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置c++标准
set(CMAKE_CXX_STANDARD 20)

# 在生成的项目文件中使用文件夹。开启文件夹组织项
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 设置引擎的目录
set(ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# 设置自定义CMake模块目录
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 设置自定义CMake模块目录
# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 设置CMake安装目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "set install path!" FORCE)

# 导出编译命令
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 生成Debug和Release项目配置
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)

# 如果是交叉编译，那么就需要把这个开启
if(CMAKE_CROSSCOMPILING)
    set(CMAKE_FIND_ROOT_PATH_BOTH true)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

# 设置Linux窗口系统
set(LINUX_WINDOW_SYSTEM XCB)

# 是否编译第三方库
set(BUILD_THIRDPARTY OFF)

# 设置是否编译Test
set(BUILD_TEST OFF)

# 设置是否使用VMA
set(VMA_ENABLE ON)

project(Solis)

# =============================================================================
# 判断编译器, 平台
# =============================================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_NAME "Clang")
    add_definitions(-D__CLANG__)
    set(CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_NAME "GCC")
    add_definitions(-D__GCC__)
    set(GCC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(COMPILER_NAME "Intel")
    add_definitions(-D__INTEL_CC__)
    set(INTEL_CC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(COMPILER_NAME "MSVC")
    add_definitions(-D__MSVC__)
    set(MSVC 1)

    if(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
        set(VC_SHORT_VER "VS2019")
    elseif(MSVC_VERSION GREATER_EQUAL 1930)
        set(VC_SHORT_VER "VS2022")
    endif()

    # 设置编译选项
    add_compile_options(/utf-8)

    # 多线程编译
    add_compile_options(/MP)

    # MSVC一般是不会设定这个东西的，所以需要给一个默认的参数
    if(NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

# 设置平台
set(PLATFORM_NAME "UNKNOW")
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(PLATFORM_NAME "Win")
    add_definitions(-D__WIN__)
    set(__WIN__ 1)

    # 设置平台名称
    set(PLATFORM_NAME "Win64")

elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "MacOS")
    set(PLATFORM_NAME "Mac")
    add_definitions(-D__MAC__)
    set(__MAC__ 1)

    set(PLATFORM_NAME "MacOS")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_NAME "Linux")
    add_definitions(-D__LINUX__)
    set(__LINUX__ 1)

    set(PLATFORM_NAME "Linux")

    # 确定 linux 窗口系统
    if(LINUX_WINDOW_SYSTEM MATCHES XCB)
        add_definitions(-DUSE_XCB)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
        # set(USE_XCB 1)
    elseif(LINUX_WINDOW_SYSTEM MATCHES XLIB)
        add_definitions(-DUSE_XLIB)
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
        # set(USE_XLIB 1)
    elseif(LINUX_WINDOW_SYSTEM MATCHES DIRECTFB)
        add_definitions(-DUSE_DIRECTFB)
        add_definitions(-DVK_USE_PLATFORM_DIRECTFB_EXT)
        # set(USE_DIRECTFB 1)
    elseif(LINUX_WINDOW_SYSTEM MATCHES WAYLAND)
        add_definitions(-DUSE_WAYLAND)
        add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
        # set(USE_WAYLAND 1)
    else()
        message(FATAL_ERROR "LINUX_WINDOW_SYSTEM ${LINUX_WINDOW_SYSTEM} not implement.")
    endif()
endif()

# 设置架构
set(PLATFORM_ARCH "UNKNOW")
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(PLATFORM_ARCH "x86_64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
    set(PLATFORM_ARCH "AMD64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM_ARCH "aarch64")
endif()

# 设定编译类型宏
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-D__DEBUG__)
    set(__DEBUG__ 1)
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-D__RELEASE__)
    set(__RELEASE__ 1)
endif()

set(PLATFORM_ARCH_PATH "${PLATFORM_NAME}/${PLATFORM_ARCH}")

# =============================================================================
# CMAKE 相关信息
# =============================================================================
message(STATUS "CMAKE_C_COMPILER_ID ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_C_COMPILER_VERSION ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")

# 设置输出目录
message(STATUS "PLATFORM_NAME:            ${PLATFORM_NAME}")
message(STATUS "PLATFORM_ARCH_PATH:       ${PLATFORM_ARCH_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX:     ${CMAKE_INSTALL_PREFIX}")



# 定义渲染后端
# set(RENDER_API_BACKEND GLFW)

# 定义渲染API平台
# set(RENDER_API_PLATFORM OpenGL)

# 设置架构
message(STATUS "CMAKE_HOST_SYSTEM_NAME:      ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# ==============================================================================
# 设置安装目录
# ==============================================================================
set(CMAKE_INSTALL_LIBDIR thirdparty/${PLATFORM_ARCH_PATH}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_INSTALL_BINDIR thirdparty/${PLATFORM_ARCH_PATH}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_INSTALL_INCLUDEDIR thirdparty/${PLATFORM_ARCH_PATH}/${CMAKE_BUILD_TYPE}/include)
set(CMAKE_INSTALL_LIBDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_BINDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_INSTALL_INCLUDEDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/thirdparty/${PLATFORM_ARCH_PATH}/${CMAKE_BUILD_TYPE})

message(STATUS "CMAKE_INSTALL_LIBDIR :          ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_BINDIR :          ${CMAKE_INSTALL_BINDIR}")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR :      ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR_FULL :     ${CMAKE_INSTALL_LIBDIR_FULL}")
message(STATUS "CMAKE_INSTALL_BINDIR_FULL :     ${CMAKE_INSTALL_BINDIR_FULL}")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR_FULL : ${CMAKE_INSTALL_INCLUDEDIR_FULL}")
message(STATUS "CMAKE_PREFIX_PATH :             ${CMAKE_PREFIX_PATH}")
# ==============================================================================
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

# 编译时候不生成动态库
if(NOT ${BUILD_THIRDPARTY})
    set(BUILD_SHARED_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)
endif()

# 只有Windows平台才有这样的设置
if(BUILD_SHARED_LIBS AND CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_INSTALL_LIBDIR thirdparty/${TARGET_PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}/bin)
endif()

# 设置第三方库
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# Source
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Test
set(TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test)

# 设置第三方库
add_subdirectory(${THIRD_PARTY_PATH})


# 加入Source集
add_subdirectory(${SOURCE_PATH})

# 加入Test集
if(BUILD_TEST)
    add_subdirectory(${TEST_PATH})
endif()

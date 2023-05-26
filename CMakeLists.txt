# CMake 最低版本号要求
################################################################################ 
cmake_minimum_required(VERSION 3.20)

# 设置c++版本要求
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)

# 在生成的项目文件中使用文件夹。开启文件夹组织项
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 设置引擎的目录
set(ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/engine)

# 设置自定义CMake模块目录
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(SOLIS_DEPEND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# 设置自定义CMake模块目录
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 设置CMake安装目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Install CACHE PATH "set install path!" FORCE)

# 导出编译命令
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Solis)

message(STATUS "CMAKE_C_COMPILER_ID ${CMAKE_C_COMPILER_ID}")

message(STATUS "CMAKE_C_COMPILER_VERSION ${CMAKE_C_COMPILER_VERSION}")

message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")

message(STATUS "CMAKE_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION}")

# 使用Unicode字符集
###############################################################################
# add_definitions("-DUNICODE -D_UNICODE")
add_compile_options(/utf-8)

if(MSVC)
    set(TARGET_PLATFORM_NAME "Win64")

    if(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
        set(VC_SHORT_VER "VS2019")
    elseif(MSVC_VERSION GREATER_EQUAL 1930)
        set(VC_SHORT_VER "VS2022")
    endif()

endif()

set(PLATFORM_ARCH_PATH "${TARGET_PLATFORM_NAME}/${VC_SHORT_VER}")

message(STATUS "\${TARGET_PLATFORM_NAME}:            ${TARGET_PLATFORM_NAME}")
message(STATUS "\${PLATFORM_ARCH_PATH}:              ${PLATFORM_ARCH_PATH}")
message(STATUS "\${CMAKE_INSTALL_PREFIX}:            ${CMAKE_INSTALL_PREFIX}")

# 生成Debug和Release项目配置
set(CMAKE_CONFIGURATION_TYPES Debug;Release)

mark_as_advanced(CMAKE_CONFIGURATION_TYPES)

# 设置编译的版本
set(CMAKE_BUILD_TYPE Debug)

# 定义渲染后端
# set(RENDER_API_BACKEND GLFW)

# 定义渲染API平台
# set(RENDER_API_PLATFORM OpenGL)

# 设置架构
message(STATUS "CMAKE_HOST_SYSTEM_NAME:      ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR:      ${CMAKE_HOST_SYSTEM_PROCESSOR}")
 
set(PLATFORM_ARCH "UNKNOW")
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(PLATFORM_ARCH "x86_64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64")
    set(PLATFORM_ARCH "AMD64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM_ARCH "aarch64")
endif()

# 设置平台
set(PLATFORM_NAME "UNKNOW")
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(PLATFORM_NAME "Win")
    add_definitions(-D__WIN__)
    set(__WIN__ 1)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Android")
    set(PLATFORM_NAME "Android")
    add_definitions(-D__ANDROID__)
    set(__ANDROID__ 1)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "MacOS")
    set(PLATFORM_NAME "Mac")
    add_definitions(-D__MAC__)
    set(__MAC__ 1)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_NAME "Linux")
    add_definitions(-D__LINUX__)
    set(__LINUX__ 1)
endif()

# 设置库的编译架构
# set(CMAKE_LIBRARY_ARCHITECTURE ${PLATFORM_ARCH_PATH})

# 设置安装目录
# ==============================================================================
set(CMAKE_INSTALL_LIBDIR thirdparty/${PLATFORM_NAME}/${PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_INSTALL_BINDIR thirdparty/${PLATFORM_NAME}/${PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_INSTALL_INCLUDEDIR thirdparty/${PLATFORM_NAME}/${PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}/include)

set(CMAKE_INSTALL_LIBDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_BINDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_INSTALL_INCLUDEDIR_FULL ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
# ==============================================================================

# set prefix path
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/thirdparty/${PLATFORM_NAME}/${PLATFORM_ARCH}/${CMAKE_BUILD_TYPE})

# 是否编译第三方库
set(BUILD_THIRDPARTY ON)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

# 编译时候不生成动态库
if(NOT ${BUILD_THIRDPARTY})
    set(BUILD_SHARED_LIBS ON)
    set(BUILD_STATIC_LIBS OFF)
endif()

# 只有Windows平台才有这样的设置
if(BUILD_SHARED_LIBS AND CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_INSTALL_LIBDIR thirdparty/${PLATFORM_NAME}/${PLATFORM_ARCH}/${CMAKE_BUILD_TYPE}/bin)
endif()

# 设置第三方库
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# 设置第三方库
add_subdirectory(${THIRD_PARTY_PATH})

# USE_VMA
set(VMA_ENABLE ON)

# Source
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source)

# 加入Source集
add_subdirectory(${SOURCE_PATH})

# Test
set(TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test)

# 加入Test集
# add_subdirectory(${TEST_PATH})

if(${BUILD_THIRDPARTY})
    # .\b2.exe address-model=64 --build-dir=build toolset=msvc-14.2 --build-type=complete stage install --prefix=install
    include(install_script.cmake)
    # 0级库(意味着一定要提前编译好
    #       因为boost有他自己独有的编译机制,无法直接从source编译,
    #       所以这里需要提前编译好, 并且编译成静态库)
    # boost(0级库只有boost, 以后这里需要写一个脚本, 自动下载boost, 并编译安装到指定路径)
    # Boost
    # set(BOOST_VERSION 1.81.0)
    # set(Boost_DIR ${CMAKE_INSTALL_PREFIX}/boost/lib/cmake/Boost-${BOOST_VERSION} CACHE INTERNAL "")
    # find_package(Boost CONFIG REQUIRED)

    # # =======================================================================
    # # 1级库(可以直接通过CMake完成编译和Install)
    # # =======================================================================
    # # CTTI
    # install_library(ctti -DCTTI_INSTALL=ON)

    # # GLM
    # install_library(glm -DGLM_INSTALL=ON)
 
    # # glfw
    # install_library(glfw -DGLFW_BUILD_DOCS=OFF)
 
    # # ASTC
 
    # # GLSLANG
    # # SPIRV 没有导出符号，所有只能用静态库
    # install_library(glslang -DBUILD_SHARED_LIBS=OFF)
 
    # # mimalloc
    # install_library(mimalloc "")
 
    # # tinygltf
    # install_library(tinygltf "")
 
    # # physfs
    # install_library(physfs "")
 
    # # IMGUI
 
    # # SPDLOG
    # install_library(spdlog "")
 
    # # STB 这个得手写cmake
    # # install_library(stb "")
 
    # # VOLK
    # install_library(volk -DVOLK_INSTALL=ON)
 
    # # VULKAN
    # find_package(Vulkan REQUIRED)
 
    # # VMA
    # install_library(VulkanMemoryAllocator "")

    # # fmt
    # install_library(fmt -DFMT_INSTALL=ON)

    # # gflags
    # install_library(gflags-2.2.2 -DBUILD_gflags_LIB=ON -DBUILD_SHARED_LIBS=ON -DINSTALL_SHARED_LIBS=ON)

    # # double conversion -> folly
    # install_library(double-conversion "")

    # # libunwind -> glog
    # if(__LINUX__)
        # install_library(libunwind-1.6.2 "")
    # endif()

    # openssl -> folly
    mingw_install_library(openssl_1_1_1t --openssldir=${CMAKE_INSTALL_PREFIX}/openssl)

    # zlib -> folly
    install_library(zlib-1.2.13 "")

    # lz4 -> folly
    # install_library(lz4-1.9.4 "")

    # zstd -> folly

    # snappy -> folly

    # =======================================================================
    # 2级库(依赖1级库)
    # =======================================================================
    # libevent(openssl) -> folly
    install_library(libevent-2.1.12 "")

    # glog -> folly
    install_library(glog-0.6.0 "")

    # =======================================================================
    # 3级库(依赖2级库, 1级库)
    # =======================================================================
    # folly
    install_library(folly -DBUILD_TESTS=OFF)

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

    # glslang
    set(glslang_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake/glslang CACHE INTERNAL "")
    find_package(glslang CONFIG REQUIRED)

    # tinygltf
    set(TinyGLTF_DIR ${CMAKE_INSTALL_LIBDIR_FULL}/cmake CACHE INTERNAL "")
    find_package(TinyGLTF CONFIG REQUIRED)
endif()
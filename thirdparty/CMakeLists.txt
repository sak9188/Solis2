if(${BUILD_THIRDPARTY})
    # .\b2.exe address-model=64 --build-dir=build toolset=msvc-14.2 --build-type=complete stage install --prefix=install
    include(install_script.cmake)
    # 0级库(意味着一定要提前编译好
    #       因为boost有他自己独有的编译机制,无法直接从source编译,
    #       所以这里需要提前编译好, 并且编译成静态库)
    # boost(以后这里需要写一个脚本, 自动下载boost, 并编译安装到指定路径)
    # Boost
    set(BOOST_VERSION 1.81.0)
    set(Boost_DIR ${CMAKE_INSTALL_PREFIX}/boost/lib/cmake/Boost-${BOOST_VERSION} CACHE INTERNAL "")
    find_package(Boost CONFIG REQUIRED)

    # openssl -> folly
    # TODO: 这个用Python来处理
    # mingw_install_library(openssl_1_1_1t --openssldir=${CMAKE_INSTALL_PREFIX}/openssl)
    set(OPENSSL_ROOT_DIR ${CMAKE_INSTALL_PREFIX}/openssl CACHE INTERNAL "")
    find_package(OpenSSL REQUIRED)
    
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
    # install_library(tinygltf -DBUILD_SHARED_LIBS=OFF)
 
    # # physfs
    # install_library(physfs "")
 
    # # IMGUI
 
    # # SPDLOG
    # install_library(spdlog "")
 
    # # STB 这个得手写cmake
    # install_library(stb "")
 
    # # VOLK
    # install_library(volk -DVOLK_INSTALL=ON)
 
    # # VULKAN
    # find_package(Vulkan REQUIRED)
 
    # # VMA
    # install_library(VulkanMemoryAllocator -DBUILD_SHARED_LIBS=OFF)

    # # fmt
    # install_library(fmt -DFMT_INSTALL=ON)

    # # gflags
    # install_library(gflags-2.2.2 "
    # -DBUILD_gflags_LIB=ON;
    # -DBUILD_SHARED_LIBS=ON;
    # -DINSTALL_SHARED_LIBS=ON;
    # ")

    # # double conversion -> folly
    # install_library(double-conversion "")

    # # libunwind -> glog
    # if(__LINUX__)
        # install_library(libunwind-1.6.2 "")
    # endif()

    # zlib -> folly
    # install_library(zlib-1.2.13 "")

    # lz4 -> folly
    # install_library(lz4-1.9.4 "")

    # zstd -> folly

    # snappy -> folly

    # libevent(openssl) -> folly
    # install_library(libevent-2.1.12 -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR})

    # SPRIV-Cross
    # install_library(SPIRV-Cross "")

    # =======================================================================
    # 2级库(依赖1级库)
    # =======================================================================
    # glog -> folly
    # install_library(glog-0.6.0 "")

    # =======================================================================
    # 3级库(依赖2级库, 1级库)
    # =======================================================================
    # folly
    # install_library(folly_2023_02_13
    # "-DBUILD_TESTS=OFF;-DCMAKE_LIBRARY_ARCHITECTURE=x86_64;-DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR};-DBUILD_SHARED_LIBS=OFF;")

    # FreeType

    # PyBind11

    # TracyClient
else()
    # ctti
    find_package(ctti CONFIG REQUIRED)

    # mimalloc
    find_package(mimalloc CONFIG REQUIRED)

    # GLM
    find_package(glm CONFIG REQUIRED)

    # VOLK
    find_package(volk CONFIG REQUIRED)

    # glfw
    find_package(glfw3 CONFIG REQUIRED)

    # FMT
    find_package(fmt CONFIG REQUIRED)

    # Boost
    find_package(Boost CONFIG REQUIRED)

    # gflags
    find_package(gflags REQUIRED)

    # stb
    find_package(stb CONFIG REQUIRED)

    # VulkanMemoryAllocator
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)

    # folly
    find_package(folly CONFIG REQUIRED)

    # glslang
    find_package(glslang CONFIG REQUIRED)

    # SPIRV-Cross
    find_package(spirv_cross_core CONFIG REQUIRED)
    find_package(spirv_cross_glsl CONFIG REQUIRED)
    find_package(spirv_cross_hlsl CONFIG REQUIRED)
    find_package(spirv_cross_msl CONFIG REQUIRED)
    find_package(spirv_cross_cpp CONFIG REQUIRED)
    find_package(spirv_cross_reflect CONFIG REQUIRED)
    find_package(spirv_cross_c CONFIG REQUIRED)

    # tinygltf
    find_package(TinyGLTF CONFIG REQUIRED)
endif()
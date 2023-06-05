# 这里以后需要变成python版本的一键install脚本
# 原因在于，cmake启动以后本身拥有的环境变量就不太好控制了
# 如果在Windows平台下，编译类unix平台的库, 那么就会在Make阶段找错编译器路径
# 这个是非常难调整的

if(__WIN__)
    set(CMAKE_BUILD_TOOLS "Visual Studio 16 2019")
    set(CMAKE_BUILD_FLAGS "-A x64")
elseif(__ANDROID__)
    set(CMAKE_BUILD_TOOLS "Ninja")
    set(CMAKE_BUILD_FLAGS "")
elseif(__LINUX__)
    set(CMAKE_BUILD_TOOLS "Ninja")
    set(CMAKE_BUILD_FLAGS "-DCFLAGS=-std=c++20 -m64")
elseif(__MAC__)
    set(CMAKE_BUILD_TOOLS "Unix Makefiles")
    set(CMAKE_BUILD_FLAGS "-DCFLAGS=-std=c++20 -m64")
endif()

macro(install_library lib_name lib_defines)

message(STATUS "build_library: ${lib_name}")
if(NOT DEFINED __ANDROID__)
    set(build_command
    "cmake . -G \"${CMAKE_BUILD_TOOLS}\" -B ./build_${CMAKE_SYSTEM_NAME}/ ${CMAKE_BUILD_FLAGS} ${CMAKE_NEXT_LINE}
-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                                                           ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}                                                   ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}                                                   ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}                                                   ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}                                           ${CMAKE_NEXT_LINE}
-DBUILD_STATIC_LIBS=ON                                                                           ${CMAKE_NEXT_LINE}
-DBUILD_SHARED_LIBS=ON                                                                           ${CMAKE_NEXT_LINE}
-DENABLE_TESTING=OFF                                                                             ${CMAKE_NEXT_LINE}
-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}                                                         ${CMAKE_NEXT_LINE}
-DCMAKE_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include                                                ${CMAKE_NEXT_LINE}
-DCMAKE_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib                                                    ${CMAKE_NEXT_LINE}
-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}                                                           ${CMAKE_NEXT_LINE}
-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}                                                       ${CMAKE_NEXT_LINE}
${lib_defines}
")
else()
    set(build_command 
        "cmake . -G \"${CMAKE_BUILD_TOOLS}\" -B ./build_${CMAKE_SYSTEM_NAME}/ ${CMAKE_BUILD_FLAGS} ${CMAKE_NEXT_LINE}
-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}                                                               ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}                                                       ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}                                                       ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}                                                       ${CMAKE_NEXT_LINE}
-DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}                                               ${CMAKE_NEXT_LINE}
-DBUILD_STATIC_LIBS=ON                                                                               ${CMAKE_NEXT_LINE}
-DBUILD_SHARED_LIBS=ON                                                                               ${CMAKE_NEXT_LINE}
-DENABLE_TESTING=OFF                                                                                 ${CMAKE_NEXT_LINE}
-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}                                                             ${CMAKE_NEXT_LINE}
-DCMAKE_INCLUDE_PATH=${CMAKE_PREFIX_PATH}/include                                                    ${CMAKE_NEXT_LINE}
-DCMAKE_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib                                                        ${CMAKE_NEXT_LINE}
-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}                                                               ${CMAKE_NEXT_LINE}
-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}                                                           ${CMAKE_NEXT_LINE}
-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}                                                       ${CMAKE_NEXT_LINE}
-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}                                                           ${CMAKE_NEXT_LINE}
-DANDROID_ABI=${ANDROID_ABI}                                                                         ${CMAKE_NEXT_LINE}
-DANDROID_PLATFORM=${ANDROID_PLATFORM}                                                               ${CMAKE_NEXT_LINE}
-DANDROID_NDK=${ANDROID_NDK}                                                                         ${CMAKE_NEXT_LINE}
-DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}                                                             ${CMAKE_NEXT_LINE}
-DANDROID_STL=${ANDROID_STL}                                                                         ${CMAKE_NEXT_LINE}
-DANDROID_CPP_FEATURES=${ANDROID_CPP_FEATURES}                                                       ${CMAKE_NEXT_LINE}
${lib_defines}
")
endif()

write_file(${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/build.${SCRIPT_SHELL_SUFFIX} ${build_command})

message(STATUS "build_command: ${build_command}")
execute_process(
    COMMAND build.${SCRIPT_SHELL_SUFFIX}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}
    RESULT_VARIABLE build_result
    OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/build.log
)
message(STATUS "work directory: ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}")

if(build_result EQUAL 0)
    message(STATUS "build ${lib_name} success")
else()
    message(FATAL_ERROR "build ${lib_name} failed")
endif()

# install 
set(install_command cmake --build ./build_${CMAKE_SYSTEM_NAME}/ --target install)

if(__WIN__)
    if(NOT DEFINED ${CMAKE_BUILD_TYPE})
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    
    set(install_command cmake --build ./build_${CMAKE_SYSTEM_NAME}/ --target install --config ${CMAKE_BUILD_TYPE})
endif()

message(STATUS "install command: ${install_command}")
execute_process(
    COMMAND ${install_command}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}
    RESULT_VARIABLE install_result
)

if(install_result EQUAL 0)
    message(STATUS "install ${lib_name} success")
else()
    message(FATAL_ERROR "install ${lib_name} failed")
endif()

endmacro(install_library)

# 类Unix平台的安装
macro(mingw_install_library lib_name lib_defines)
    # 这里需要先保证mingw64的环境变量已经设置好了
    # 所以这里有可能需要执行一段检查路径搜索的代码
    # 在上面

    message(STATUS "build_library: ${lib_name}")
    set(build_command
        perl ./Configure mingw64 --prefix=${CMAKE_INSTALL_PREFIX} ${lib_defines}
    )

    message(STATUS "build_command: ${build_command}")
    execute_process(
        COMMAND ${build_command}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}
        RESULT_VARIABLE build_result
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/build.log
    )
    message(STATUS "work directory: ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}")

    if(build_result EQUAL 0)
        message(STATUS "build ${lib_name} success")
    else()
        message(FATAL_ERROR "build ${lib_name} failed")
    endif()

    # install 
    set(install_command make install)
    execute_process(
        COMMAND ${install_command}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}
        RESULT_VARIABLE install_result
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}/build_install.log
    )
    
    message(STATUS "install command: ${install_command}")
    if(install_result EQUAL 0)
        message(STATUS "install ${lib_name} success")
    else()
        message(FATAL_ERROR "install ${lib_name} failed")
    endif()
endmacro(mingw_install_library)



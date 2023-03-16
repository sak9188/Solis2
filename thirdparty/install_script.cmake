if(__WIN__)
    set(CMAKE_BUILD_TOOLS "Visual Studio 16 2019")
elseif(__ANDROID__)
    # TODO: 这里有可能是其他的构建工具，不过先不管了
    set(CMAKE_BUILD_TOOLS "Unix Makefiles")
elseif(__LINUX__)
    set(CMAKE_BUILD_TOOLS "Unix Makefiles")
elseif(__MAC__)
    set(CMAKE_BUILD_TOOLS "Unix Makefiles")
endif()



macro(install_library lib_name lib_defines)
    message(STATUS "build_library: ${lib_name}")
    set(build_command 
        cmake . -G "${CMAKE_BUILD_TOOLS}" -B ./build/ -A x64
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
        -DCMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}
        -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}
        -DBUILD_SHARED_LIBS=ON
        -DBUILD_STATIC_LIBS=ON
        -DENABLE_TESTING=OFF
        ${lib_defines}
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
    set(install_command cmake --build ./build/ --target install)
    execute_process(
        COMMAND ${install_command}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}
        RESULT_VARIABLE install_result
    )
    
    message(STATUS "install command: ${install_command}")
    if(install_result EQUAL 0)
        message(STATUS "install ${lib_name} success")
    else()
        message(FATAL_ERROR "install ${lib_name} failed")
    endif()

endmacro(install_library)

# 类Unix平台的安装
macro(mingw_install_library lib_name)
    # 这里需要先保证mingw64的环境变量已经设置好了
    # 所以这里有可能需要执行一段检查路径搜索的代码
    # 在上面

    message(STATUS "build_library: ${lib_name}")
    set(build_command 
        D:/msys64/msys2_shell.cmd -mingw64 ./configure --prefix=${CMAKE_INSTALL_PREFIX}
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
    )
    
    message(STATUS "install command: ${install_command}")
    if(install_result EQUAL 0)
        message(STATUS "install ${lib_name} success")
    else()
        message(FATAL_ERROR "install ${lib_name} failed")
    endif()
endmacro(mingw_install_library)



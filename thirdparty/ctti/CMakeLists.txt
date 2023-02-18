project(ctti)

option(CTTI_BUILD_TESTS "Build ctti feature tests" OFF)
option(CTTI_BUILD_EXAMPLES "Build ctti examples" OFF)

include(cmake/vs_source_groups.cmake)

set(CTTI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wall -Werror")
endif()

add_library(ctti INTERFACE)
target_include_directories(ctti INTERFACE
    $<BUILD_INTERFACE:${CTTI_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include/>)

if(${CTTI_INSTALL})
    install(
        TARGETS ctti
        EXPORT ctti-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
      EXPORT ctti-targets
      FILE ctti-config.cmake
      NAMESPACE ctti::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
    )

    install(DIRECTORY include/ctti DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if(CTTI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CTTI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

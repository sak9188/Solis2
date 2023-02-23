project(solis_app CXX)

# 设置目录
set(PROJECT_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(PROJECT_SOURCE_PATH ${CMAKE_CURRENT_LIST_DIR})

# 收集文件
file(GLOB_RECURSE PROJECT_SOURCES
    ${PROJECT_SOURCE_PATH}/*.cpp
)

file(GLOB_RECURSE PROJECT_HEADERS
    ${PROJECT_INCLUDE_PATH}/*.h
    ${PROJECT_INCLUDE_PATH}/*.hpp
)

# 对文件进行分组
source_group(TREE ${PROJECT_SOURCE_PATH}
    FILES ${PROJECT_SOURCES}
)

source_group(TREE ${PROJECT_INCLUDE_PATH}
    FILES ${PROJECT_HEADERS}
)

# 设置编译宏
add_executable(solis_app ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# 设置依赖
find_package(glfw3 CONFIG REQUIRED)

# 设置依赖
target_link_libraries(
    solis_app
    PUBLIC
        solis_core
        glfw
)

message(STATUS "CMAKE_INSTALL_INCLUDEDIR_FULL: ${CMAKE_INSTALL_INCLUDEDIR_FULL}")

target_include_directories(
    solis_app 
    PUBLIC
        ${CMAKE_INSTALL_INCLUDEDIR_FULL}
        ${ENGINE_SOURCE_DIR}
)

# 项目分租
set_target_properties(
    solis_app 
    PROPERTIES
        FOLDER "Runtime" 
)

set_target_properties(
    solis_app 
    PROPERTIES
        OUTPUT_NAME "solis_app"
)


set_target_properties(
    solis_app 
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
)

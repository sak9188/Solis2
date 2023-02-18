project(solis_core CXX)

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
add_definitions(-DSOLIS_CORE_LIB)

# 编译这个lib
add_library(solis_core SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(
    solis_core
    PUBLIC
        glfw
        glm
        stb
        Folly::folly
)

target_include_directories(
    solis_core 
    PUBLIC
        ${ENGINE_SOURCE_DIR}
)

# 项目分租
set_target_properties(
    solis_core 
    PROPERTIES
        FOLDER "Runtime" 
)

set_target_properties(
    solis_core 
    PROPERTIES
        OUTPUT_NAME "libsolis_core"
    
)

project(test_windows_stack CXX)
set(PROJECT_NAME test_windows_stack)

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

# 编译这个lib
# add_library(solis_core SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_executable(test_windows_stack ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        ${ENGINE_LIB_BUILD_LIB}
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
        ${ENGINE_LIB_BUILD_INCLUDE}
        ${ENGINE_SOURCE_PATH}
)

# 项目分租
set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
        FOLDER "Test" 
)

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES
        OUTPUT_NAME "test_windows_stack"
    
)
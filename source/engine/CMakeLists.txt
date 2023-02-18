# SET ENGINE SOURCE
set(ENGINE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("core")

add_subdirectory("inputs")

add_subdirectory("physics")

add_subdirectory("ui")

add_subdirectory("utils")

add_subdirectory("world")

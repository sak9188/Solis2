set(ENGINE_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# 引擎运行时
set(ENGINE_RUNTIME ${ENGINE_SOURCE_PATH}/engine)

# 引擎Editor
set(ENGINE_EDITOR ${ENGINE_SOURCE_PATH}/editor)

# 引擎APP
set(ENGINE_APP ${ENGINE_SOURCE_PATH}/app)

if(NOT ${BUILD_THIRDPARTY})
    add_subdirectory(${ENGINE_RUNTIME})

    add_subdirectory(${ENGINE_EDITOR})

    add_subdirectory(${ENGINE_APP})
endif()

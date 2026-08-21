# CMake configuration for the Algine glm module.
if(NOT TARGET algine_glm)
    file(GLOB_RECURSE ALGINE_GLM_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/glm/*.cpp")
    if(ALGINE_GLM_SOURCES)
        add_library(alGINE_glm ${ALGINE_GLM_SOURCES})
        target_include_directories(alGINE_glm PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/glm")
    else()
        add_library(alGINE_glm INTERFACE)
        target_include_directories(alGINE_glm INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/glm")
    endif()
    set_target_properties(alGINE_glm PROPERTIES CXX_STANDARD 17)
endif()

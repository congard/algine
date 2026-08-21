# CMake configuration for the Algine core module.
if(NOT TARGET algine_core)
    file(GLOB_RECURSE ALGINE_CORE_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/core/*.cpp")
    if(ALGINE_CORE_SOURCES)
        add_library(alGINE_core ${ALGINE_CORE_SOURCES})
        target_include_directories(alGINE_core PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/core")
    else()
        add_library(alGINE_core INTERFACE)
        target_include_directories(alGINE_core INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/core")
    endif()
    set_target_properties(alGINE_core PROPERTIES CXX_STANDARD 17)
endif()

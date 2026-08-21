# CMake configuration for the Algine common module.
if(NOT TARGET algine_common)
    file(GLOB_RECURSE ALGINE_COMMON_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/common/*.cpp")
    if(ALGINE_COMMON_SOURCES)
        add_library(alGINE_common ${ALGINE_COMMON_SOURCES})
        target_include_directories(alGINE_common PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine")
    else()
        add_library(alGINE_common INTERFACE)
        target_include_directories(alGINE_common INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine")
    endif()
    set_target_properties(alGINE_common PROPERTIES CXX_STANDARD 17)
endif()

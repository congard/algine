# CMake configuration for the Algine ext module.
if(NOT TARGET algine_ext)
    file(GLOB_RECURSE ALGINE_EXT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/ext/*.cpp")
    if(ALGINE_EXT_SOURCES)
        add_library(alGINE_ext ${ALGINE_EXT_SOURCES})
        target_include_directories(alGINE_ext PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/ext")
    else()
        add_library(alGINE_ext INTERFACE)
        target_include_directories(alGINE_ext INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/ext")
    endif()
    set_target_properties(alGINE_ext PROPERTIES CXX_STANDARD 17)
endif()

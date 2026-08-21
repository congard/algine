# CMake configuration for the Algine desktop module.
if(NOT TARGET algine_desktop)
    file(GLOB_RECURSE ALGINE_DESKTOP_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/desktop/*.cpp")
    if(ALGINE_DESKTOP_SOURCES)
        add_library(alGINE_desktop ${ALGINE_DESKTOP_SOURCES})
        target_include_directories(alGINE_desktop PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/desktop")
    else()
        add_library(alGINE_desktop INTERFACE)
        target_include_directories(alGINE_desktop INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/desktop")
    endif()
    set_target_properties(alGINE_desktop PROPERTIES CXX_STANDARD 17)
endif()

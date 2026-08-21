# CMake configuration for the Algine std module.
if(NOT TARGET algine_std)
    file(GLOB_RECURSE ALGINE_STD_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/std/*.cpp")
    if(ALGINE_STD_SOURCES)
        add_library(alGINE_std ${ALGINE_STD_SOURCES})
        target_include_directories(alGINE_std PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/std")
    else()
        add_library(alGINE_std INTERFACE)
        target_include_directories(alGINE_std INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/std")
    endif()
    set_target_properties(alGINE_std PROPERTIES CXX_STANDARD 17)
endif()

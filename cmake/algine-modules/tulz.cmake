# CMake configuration for the Algine tulz module.
if(NOT TARGET algine_tulz)
    file(GLOB_RECURSE ALGINE_TULZ_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/tulz/*.cpp")
    if(ALGINE_TULZ_SOURCES)
        add_library(alGINE_tulz ${ALGINE_TULZ_SOURCES})
        target_include_directories(alGINE_tulz PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/tulz")
    else()
        add_library(alGINE_tulz INTERFACE)
        target_include_directories(alGINE_tulz INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/tulz")
    endif()
    set_target_properties(alGINE_tulz PROPERTIES CXX_STANDARD 17)
endif()

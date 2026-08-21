# CMake configuration for the Algine android module.
if(NOT TARGET algine_android)
    file(GLOB_RECURSE ALGINE_ANDROID_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/android/*.cpp")
    if(ALGINE_ANDROID_SOURCES)
        add_library(alGINE_android ${ALGINE_ANDROID_SOURCES})
        target_include_directories(alGINE_android PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/android")
    else()
        add_library(alGINE_android INTERFACE)
        target_include_directories(alGINE_android INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/android")
    endif()
    set_target_properties(alGINE_android PROPERTIES CXX_STANDARD 17)
endif()

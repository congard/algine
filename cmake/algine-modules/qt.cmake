# CMake configuration for the Algine qt module.
if(NOT TARGET algine_qt)
    file(GLOB_RECURSE ALGINE_QT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/qt/*.cpp")
    if(ALGINE_QT_SOURCES)
        add_library(alGINE_qt ${ALGINE_QT_SOURCES})
        target_include_directories(alGINE_qt PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/qt")
    else()
        add_library(alGINE_qt INTERFACE)
        target_include_directories(alGINE_qt INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/qt")
    endif()
    set_target_properties(alGINE_qt PROPERTIES CXX_STANDARD 17)
endif()

# CMake configuration for the Algine widgets module.
if(NOT TARGET algine_widgets)
    file(GLOB_RECURSE ALGINE_WIDGETS_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/widgets/*.cpp")
    if(ALGINE_WIDGETS_SOURCES)
        add_library(alGINE_widgets ${ALGINE_WIDGETS_SOURCES})
        target_include_directories(alGINE_widgets PUBLIC
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/core/widgets")
    else()
        add_library(alGINE_widgets INTERFACE)
        target_include_directories(alGINE_widgets INTERFACE
            "${CMAKE_SOURCE_DIR}/include/common"
            "${CMAKE_SOURCE_DIR}/include/common/algine/core/widgets")
    endif()
    set_target_properties(alGINE_widgets PROPERTIES CXX_STANDARD 17)
endif()

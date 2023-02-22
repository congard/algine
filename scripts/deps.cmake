# Variables:
#   source_dir_${target}        CMakeLists.txt location for target ${target}
# Functions:
#   post_populate_${target}     Function which will be called after FetchContent_Populate

# https://stackoverflow.com/questions/56329088/cmake-fetchcontent-downloads-external-dependencies-multiple-times

include(FetchContent)

get_filename_component(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps" REALPATH)

function(add_dep target)
    # skip if dependency is disabled (for example glew on Android)
    if (NOT ${target} IN_LIST deps_enabled)
        return()
    endif()

    FetchContent_Declare(
        ${target}
        ${ARGN}
    )

    FetchContent_GetProperties(${target})

    if (NOT ${target}_POPULATED)
        FetchContent_Populate(${target})

        # check if there exists post populate function; if so, call it
        if (COMMAND post_populate_${target})
            cmake_language(CALL post_populate_${target})
        endif()

        message(VERBOSE "${target}_SOURCE_DIR: ${${target}_SOURCE_DIR}")

        set(${target}_SOURCE_DIR "${${target}_SOURCE_DIR}" PARENT_SCOPE)
        set(_full_source_dir "${${target}_SOURCE_DIR}/${source_dir_${target}}")

        if (EXISTS "${_full_source_dir}/CMakeLists.txt")
            add_subdirectory("${_full_source_dir}" "deps-build/${target}")
        endif()
    endif()
endfunction()

function(post_populate_glew)
    if (UNIX)
        execute_process(
            COMMAND bash -c "make extensions"
            WORKING_DIRECTORY "${glew_SOURCE_DIR}"
        )
    endif()
endfunction()

set(deps_enabled
    tulz assimp glm freetype pugixml lua sol2 solgen
    stb_image stb_image_write nlohmann_json)

set(source_dir_glew build/cmake)

if (NOT ANDROID)
    set(BUILD_UTILS OFF) # glew flag

    if (BUILD_SHARED_LIBS)
        set(GLEW_LIBRARY glew)
    else()
        set(GLEW_LIBRARY glew_s)
        add_definitions(-DGLEW_STATIC)
    endif()

    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)

    set(deps_enabled
        ${deps_enabled} glew glfw)
endif()

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_NO_EXPORT ON)

add_dep(
    tulz
    GIT_REPOSITORY  https://github.com/congard/tulz.git
    GIT_TAG         5c821acff8468e27ff233336645ab21138b5866a
)

add_dep(
    assimp
    GIT_REPOSITORY  https://github.com/assimp/assimp.git
    GIT_TAG         9519a62dd20799c5493c638d1ef5a6f484e5faf1 # 5.2.5
)

add_dep(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git
    GIT_TAG         bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8
)

add_dep(
    freetype
    GIT_REPOSITORY  https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG         e8ebfe988b5f57bfb9a3ecb13c70d9791bce9ecf # 2.12.1
)

add_dep(
    pugixml
    GIT_REPOSITORY  https://github.com/zeux/pugixml.git
    GIT_TAG         dd50fa5b45ab8d58d6c27566c2eaf04a8b7e5841 # 1.12.1
)

add_dep(
    lua
    GIT_REPOSITORY  https://github.com/walterschell/Lua.git
    GIT_TAG         14f98e5fdcde3ccd7ea9188181dd7e50660a2999
)

add_dep(
    sol2
    GIT_REPOSITORY  https://github.com/ThePhD/sol2.git
    GIT_TAG         eba86625b707e3c8c99bbfc4624e51f42dc9e561 # 3.3.0
)

add_dep(
    solgen
    GIT_REPOSITORY  https://github.com/congard/solgen.git
    GIT_TAG         14e77628eb3035228b8da228b18e8b0801a2ac7f
)

add_dep(
    glew
    GIT_REPOSITORY  https://github.com/nigels-com/glew.git
    GIT_TAG         9fb23c3e61cbd2d581e33ff7d8579b572b38ee26 # 2.2.0
)

add_dep(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         7482de6071d21db77a7236155da44c172a7f6c9e # 3.3.8
)

add_dep(
    stb_image
    URL https://raw.githubusercontent.com/nothings/stb/8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55/stb_image.h
    DOWNLOAD_NO_EXTRACT TRUE
)

add_dep(
    stb_image_write
    URL https://raw.githubusercontent.com/nothings/stb/8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55/stb_image_write.h
    DOWNLOAD_NO_EXTRACT TRUE
)

add_dep(
    nlohmann_json
    URL https://raw.githubusercontent.com/nlohmann/json/bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d/single_include/nlohmann/json.hpp
    DOWNLOAD_NO_EXTRACT TRUE
)

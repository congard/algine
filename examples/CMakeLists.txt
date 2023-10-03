cmake_minimum_required(VERSION 3.18)
project(algine_examples)

set(CMAKE_CXX_STANDARD 20)

# silence warnings from sol2
add_compile_options(-Wno-unknown-warning-option)

include(../cmake/mold.cmake)
try_use_mold()

macro(set_global name value)
    set(${name} ${value} CACHE INTERNAL ${name})
endmacro()

set_global(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set_global(ASSIMP_BUILD_OBJ_IMPORTER ON)
set_global(ASSIMP_BUILD_ZLIB OFF)
set_global(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set_global(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(../ algine)

add_subdirectory(BasicWidgets)
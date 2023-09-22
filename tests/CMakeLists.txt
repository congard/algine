cmake_minimum_required(VERSION 3.10)
project(algine_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(FetchContent)

if (UNIX)
    set(DEPS_DIR "deps")
elseif (WIN32)
    set(DEPS_DIR "deps-win")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

get_filename_component(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${DEPS_DIR}" REALPATH)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# build gtest as static library
set(BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(googletest)

# restore old value
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD})

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

function(add_gtest name src)
    add_executable(${name} "${src}")

    target_link_libraries(${name} gtest gtest_main algine)
    target_include_directories(${name} PUBLIC ../include)

    add_test(NAME ${name} COMMAND ${name})

    algine_target_mklink(
            TARGET ${name}
            ALGINE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/algine")
endfunction()

add_gtest(ColorTest src/algine/common/core/ColorTest.cpp)

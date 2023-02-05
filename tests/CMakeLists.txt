cmake_minimum_required(VERSION 3.10)
project(algine_tests)

set(CMAKE_CXX_STANDARD 17)

# silence warnings from sol2
add_compile_options(-Wno-unknown-warning-option)

if (UNIX AND NOT ANDROID)
    execute_process(COMMAND bash -c "which mold > /dev/null" RESULT_VARIABLE moldRes)

    if (${moldRes} EQUAL 0)
        message(STATUS "mold was found and will be used for linking")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=mold")
    endif()
endif()

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_ZLIB OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(../ cmake-build-debug)

if (ALGINE_DEBUG_ENABLE_SANDBOX)
    include(src/sandbox/targets.cmake)
endif()

function(add_test name src)
    add_executable(${name} ${src})
    target_include_directories(${name} PRIVATE src)
    target_link_libraries(${name} PRIVATE algine)
endfunction()

add_test(ColorTest src/algine/common/core/ColorTest.cpp)

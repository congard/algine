cmake_minimum_required(VERSION 3.18)
project(algine)

set(CMAKE_CXX_STANDARD 20)

message(VERBOSE "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(VERBOSE "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

set(ALGINE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL ALGINE_SOURCE_DIR)

if (UNIX)
    add_compile_options(-fPIC)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    if (NOT MSVC)
        message(STATUS "Algine debug mode - adding -g option")
        add_compile_options("-g")
    endif()

    if (NOT DEFINED ALGINE_SECURE_OPERATIONS)
        message(STATUS "Algine SOP set to ON")
        set(ALGINE_SECURE_OPERATIONS ON)
    endif()
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Algine release mode")

    # should inline functions like bind(), unbind() etc
    if (NOT MSVC)
        add_compile_options("-flto")
    endif()

    if (NOT DEFINED ALGINE_SECURE_OPERATIONS)
        message(STATUS "Algine SOP set to OFF")
        set(ALGINE_SECURE_OPERATIONS OFF)
    endif()
else()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}\nBut you can choose only Debug or Release build type")
endif()

if (ALGINE_SECURE_OPERATIONS)
    if (NOT DEFINED ALGINE_SOP_LEVEL)
        set(ALGINE_SOP_LEVEL 0)
    endif()

    add_definitions(-DALGINE_SECURE_OPERATIONS)
    add_definitions(-DALGINE_SOP_LEVEL=${ALGINE_SOP_LEVEL})

    message(STATUS "Algine SOP level is ${ALGINE_SOP_LEVEL}")
endif()

# Qt
if (ALGINE_QT_PLATFORM)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets OpenGL REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL REQUIRED)

    set(CMAKE_AUTOMOC ON)

    set(ALGINE_QT_LIBS
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::OpenGL)

    message(STATUS "Using Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")
endif()

include(cmake/deps.cmake)

# pack built-in glsl resources
include(cmake/GLSLResources.cmake)
glsl_pack_all()

# lua
function(build_lua_interp out_binary)
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/tools/lua_interp")
    set(lua_interp_dir "${CMAKE_CURRENT_LIST_DIR}/tools/lua_interp")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -B ${output_dir}
            -G Ninja
            -D CMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -D DEPS_DIR=${ALGINE_DEPS_DIR}
        WORKING_DIRECTORY ${lua_interp_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${output_dir} --target lua_interp -- -j 4
        WORKING_DIRECTORY ${lua_interp_dir})
    
    set(${out_binary} "${output_dir}/lua_interp" PARENT_SCOPE)
endfunction()

if (NOT ALGINE_LUA_PATH)
    message(STATUS "ALGINE_LUA_PATH is not specified, building lua_interp")
    build_lua_interp(ALGINE_LUA_PATH)
    message(STATUS "ALGINE_LUA_PATH=${ALGINE_LUA_PATH}")
endif()

file(READ src/common/core/lua/core.lua Lua_core_lua)

configure_file(src/common/core/lua/CoreLua.h.in src/common/core/lua/CoreLua.h)

set(SOLGEN_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/src/solgen)

# include paths
set(PRIVATE_INCLUDES
    src/common
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-modules
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-shaders
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/painter
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua

    ${assimp_SOURCE_DIR}/include
    ${freetype_SOURCE_DIR}/include
    ${pugixml_SOURCE_DIR}/src
    ${lua_SOURCE_DIR}/lua-5.4.4/include
    ${stb_image_SOURCE_DIR}
    ${stb_image_write_SOURCE_DIR}

    ${SOLGEN_OUTPUT_PATH})

set(PUBLIC_INCLUDES
    ${tulz_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${sol2_SOURCE_DIR}/include
    ${nlohmann_json_SOURCE_DIR}
    include/common)

if (ALGINE_QT_PLATFORM)
    set(PRIVATE_INCLUDES ${PRIVATE_INCLUDES}
        src/qt)

    set(PUBLIC_INCLUDES ${PUBLIC_INCLUDES}
        ${glew_SOURCE_DIR}/include # TODO: mark PRIVATE in the future (see Texture.h)
        include/qt)
elseif (NOT ANDROID)
    set(PRIVATE_INCLUDES ${PRIVATE_INCLUDES}
        src/desktop
        ${glfw_SOURCE_DIR}/include)

    set(PUBLIC_INCLUDES ${PUBLIC_INCLUDES}
        ${glew_SOURCE_DIR}/include # TODO: mark PRIVATE in the future (see Texture.h)
        include/desktop)
else()
    set(PUBLIC_INCLUDES ${PUBLIC_INCLUDES}
        ${tulz_SOURCE_DIR}/android/include
        include/android)
endif()

if (MSVC OR ANDROID)
    set(PRIVATE_INCLUDES ${PRIVATE_INCLUDES} ${assimp_BINARY_DIR}/include)
endif()

# solgen
include(cmake/solgen.cmake)
solgen(ALGINE_SOLGEN_SOURCES)

# collect project sources
function(collectSources output headersPath sourcesPath)
    file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "${headersPath}/*.h"
        "${sourcesPath}/*.h"
        "${sourcesPath}/*.cpp"
        "${sourcesPath}/*.c")
    set(${output} ${${output}} ${files} PARENT_SCOPE)
endfunction()

collectSources(ALGINE_CORE_SOURCES "include/common/algine/core" "src/common/core")
collectSources(ALGINE_STD_SOURCES "include/common/algine/std" "src/common/std")
collectSources(ALGINE_EXT_SOURCES "include/common/algine/ext" "src/common/ext")

set(ALGINE_CORE_SOURCES ${ALGINE_CORE_SOURCES}
    ${pugixml_SOURCE_DIR}/src/pugixml.cpp

    src/common/core/glsl-modules/GLSLModules.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-modules/GLSLModules.h
    src/common/core/glsl-shaders/GLSLShaders.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-shaders/GLSLShaders.h
    src/common/core/painter/PainterShaders.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/painter/PainterShaders.h
    src/common/core/lua/CoreLua.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/CoreLua.h

    src/common/internal/ConfigStrings.h

    include/common/algine/platform.h
    include/common/algine/types.h
    include/common/algine/gl.h)

set(ALGINE_STD_SOURCES ${ALGINE_STD_SOURCES}
    include/common/algine/constants/ShadowShader.h
    include/common/algine/constants/QuadShader.h
    include/common/algine/constants/CubemapShader.h)

if (ALGINE_QT_PLATFORM)
    collectSources(ALGINE_QT_SOURCES "include/qt/algine" "src/qt")
elseif (NOT ANDROID)
    collectSources(ALGINE_DESKTOP_SOURCES "include/desktop/algine" "src/desktop")
else()
    collectSources(ALGINE_ANDROID_SOURCES "include/android/algine" "src/android")
endif()

add_library(algine
    ${ALGINE_CORE_SOURCES}
    ${ALGINE_STD_SOURCES}
    ${ALGINE_EXT_SOURCES}
    ${ALGINE_QT_SOURCES}
    ${ALGINE_DESKTOP_SOURCES}
    ${ALGINE_ANDROID_SOURCES}
    ${ALGINE_SOLGEN_SOURCES})

set_target_properties(algine PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(algine PRIVATE ${PRIVATE_INCLUDES})
target_include_directories(algine PUBLIC ${PUBLIC_INCLUDES})

# Qt definitions
if (ALGINE_QT_PLATFORM)
    target_compile_definitions(algine PUBLIC ALGINE_QT_PLATFORM)
endif()

# export symbols
if (WIN32)
    target_compile_definitions(algine PRIVATE ALGINE_BUILD_LIB)
endif()

# linking
if (ANDROID)
    target_link_libraries(algine assimp android log GLESv3 EGL tulz freetype lua_static)
elseif (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw opengl32 tulz freetype lua_static ${ALGINE_QT_LIBS})
    target_link_options(algine PRIVATE -Wl,-FORCE)
elseif (UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw GL tulz freetype lua_static ${ALGINE_QT_LIBS})
endif()

# include some helpful scripts for Windows
if (WIN32)
    include(cmake/win32/PostBuild.cmake)
endif()

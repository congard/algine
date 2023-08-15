cmake_minimum_required(VERSION 3.18)
project(algine)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

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
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

    set(CMAKE_AUTOMOC ON)
    set(ALGINE_QT_LIBS Qt${QT_VERSION_MAJOR}::Widgets)
endif()

include(scripts/deps.cmake)

# built-in glsl modules
# TODO: move to another file
file(READ src/common/core/glsl-modules/BoneSystem.glsl GLSLModules_BoneSystem)
file(READ src/common/core/glsl-modules/NormalMapping.vs.glsl GLSLModules_NormalMapping_vs)
file(READ src/common/core/glsl-modules/NormalMapping.fs.glsl GLSLModules_NormalMapping_fs)
file(READ src/common/core/glsl-modules/SSR.glsl GLSLModules_SSR)

file(READ src/common/core/glsl-modules/fresnel.glsl GLSLModules_fresnel)

file(READ src/common/core/glsl-modules/ToneMapping/all.glsl GLSLModules_ToneMapping_all)
file(READ src/common/core/glsl-modules/ToneMapping/aces.glsl GLSLModules_ToneMapping_aces)
file(READ src/common/core/glsl-modules/ToneMapping/exposure.glsl GLSLModules_ToneMapping_exposure)
file(READ src/common/core/glsl-modules/ToneMapping/filmic.glsl GLSLModules_ToneMapping_filmic)
file(READ src/common/core/glsl-modules/ToneMapping/lottes.glsl GLSLModules_ToneMapping_lottes)
file(READ src/common/core/glsl-modules/ToneMapping/reinhard.glsl GLSLModules_ToneMapping_reinhard)
file(READ src/common/core/glsl-modules/ToneMapping/reinhard2.glsl GLSLModules_ToneMapping_reinhard2)
file(READ src/common/core/glsl-modules/ToneMapping/uchimura.glsl GLSLModules_ToneMapping_uchimura)
file(READ src/common/core/glsl-modules/ToneMapping/uncharted2.glsl GLSLModules_ToneMapping_uncharted2)
file(READ src/common/core/glsl-modules/ToneMapping/unreal.glsl GLSLModules_ToneMapping_unreal)

file(READ src/common/core/glsl-modules/Blending/all.glsl GLSLModules_Blending_all)
file(READ src/common/core/glsl-modules/Blending/add.glsl GLSLModules_Blending_add)
file(READ src/common/core/glsl-modules/Blending/average.glsl GLSLModules_Blending_average)
file(READ src/common/core/glsl-modules/Blending/color-burn.glsl GLSLModules_Blending_color_burn)
file(READ src/common/core/glsl-modules/Blending/color-dodge.glsl GLSLModules_Blending_color_dodge)
file(READ src/common/core/glsl-modules/Blending/darken.glsl GLSLModules_Blending_darken)
file(READ src/common/core/glsl-modules/Blending/difference.glsl GLSLModules_Blending_difference)
file(READ src/common/core/glsl-modules/Blending/exclusion.glsl GLSLModules_Blending_exclusion)
file(READ src/common/core/glsl-modules/Blending/glow.glsl GLSLModules_Blending_glow)
file(READ src/common/core/glsl-modules/Blending/hard-light.glsl GLSLModules_Blending_hard_light)
file(READ src/common/core/glsl-modules/Blending/hard-mix.glsl GLSLModules_Blending_hard_mix)
file(READ src/common/core/glsl-modules/Blending/lighten.glsl GLSLModules_Blending_lighten)
file(READ src/common/core/glsl-modules/Blending/linear-burn.glsl GLSLModules_Blending_linear_burn)
file(READ src/common/core/glsl-modules/Blending/linear-dodge.glsl GLSLModules_Blending_linear_dodge)
file(READ src/common/core/glsl-modules/Blending/linear-light.glsl GLSLModules_Blending_linear_light)
file(READ src/common/core/glsl-modules/Blending/multiply.glsl GLSLModules_Blending_multiply)
file(READ src/common/core/glsl-modules/Blending/negation.glsl GLSLModules_Blending_negation)
file(READ src/common/core/glsl-modules/Blending/normal.glsl GLSLModules_Blending_normal)
file(READ src/common/core/glsl-modules/Blending/overlay.glsl GLSLModules_Blending_overlay)
file(READ src/common/core/glsl-modules/Blending/phoenix.glsl GLSLModules_Blending_phoenix)
file(READ src/common/core/glsl-modules/Blending/pin-light.glsl GLSLModules_Blending_pin_light)
file(READ src/common/core/glsl-modules/Blending/reflect.glsl GLSLModules_Blending_reflect)
file(READ src/common/core/glsl-modules/Blending/screen.glsl GLSLModules_Blending_screen)
file(READ src/common/core/glsl-modules/Blending/soft-light.glsl GLSLModules_Blending_soft_light)
file(READ src/common/core/glsl-modules/Blending/subtract.glsl GLSLModules_Blending_subtract)
file(READ src/common/core/glsl-modules/Blending/vivid-light.glsl GLSLModules_Blending_vivid_light)

file(READ src/common/core/glsl-modules/Shading/all.glsl GLSLModules_Shading_all)
file(READ src/common/core/glsl-modules/Shading/attenuation.glsl GLSLModules_Shading_attenuation)
file(READ src/common/core/glsl-modules/Shading/diffuseLambert.glsl GLSLModules_Shading_diffuseLambert)
file(READ src/common/core/glsl-modules/Shading/diffuseOrenNayar.glsl GLSLModules_Shading_diffuseOrenNayar)
file(READ src/common/core/glsl-modules/Shading/specularAnisotropic.glsl GLSLModules_Shading_specularAnisotropic)
file(READ src/common/core/glsl-modules/Shading/specularBlinnPhong.glsl GLSLModules_Shading_specularBlinnPhong)
file(READ src/common/core/glsl-modules/Shading/specularCookTorrance.glsl GLSLModules_Shading_specularCookTorrance)
file(READ src/common/core/glsl-modules/Shading/specularPhong.glsl GLSLModules_Shading_specularPhong)
file(READ src/common/core/glsl-modules/Shading/dirLightShadow.glsl GLSLModules_Shading_dirLightShadow)
file(READ src/common/core/glsl-modules/Shading/pointLightShadow.glsl GLSLModules_Shading_pointLightShadow)

file(READ src/common/core/glsl-modules/DOF/all.glsl GLSLModules_DOF_all)
file(READ src/common/core/glsl-modules/DOF/cinematicCoC.glsl GLSLModules_DOF_cinematicCoC)
file(READ src/common/core/glsl-modules/DOF/linearCoC.glsl GLSLModules_DOF_linearCoC)

file(READ src/common/core/glsl-modules/Luminance/all.glsl GLSLModules_Luminance_all)
file(READ src/common/core/glsl-modules/Luminance/luminance.glsl GLSLModules_Luminance_luminance)
file(READ src/common/core/glsl-modules/Luminance/luminanceHSP.glsl GLSLModules_Luminance_luminanceHSP)
file(READ src/common/core/glsl-modules/Luminance/luminanceW3.glsl GLSLModules_Luminance_luminanceW3)

configure_file(src/common/core/glsl-modules/GLSLModules.h.in src/common/core/glsl-modules/GLSLModules.h)

# built-in glsl shaders
file(READ src/common/core/glsl-shaders/Blur.fs.glsl GLSLShaders_Blur_fs)
file(READ src/common/core/glsl-shaders/Quad.vs.glsl GLSLShaders_Quad_vs)
file(READ src/common/core/glsl-shaders/Shadow.fs.glsl GLSLShaders_Shadow_fs)
file(READ src/common/core/glsl-shaders/Shadow.gs.glsl GLSLShaders_Shadow_gs)
file(READ src/common/core/glsl-shaders/Shadow.vs.glsl GLSLShaders_Shadow_vs)

configure_file(src/common/core/glsl-shaders/GLSLShaders.h.in src/common/core/glsl-shaders/GLSLShaders.h)

# painter shaders
file(READ src/common/core/painter/painter.vs.glsl Painter_painter_vs)
file(READ src/common/core/painter/fill.fs.glsl Painter_fill_fs)
file(READ src/common/core/painter/circle-fill.fs.glsl Painter_circle_fill_fs)
file(READ src/common/core/painter/roundRect-fill.fs.glsl Painter_round_rect_fill_fs)
file(READ src/common/core/painter/text-fill.fs.glsl Painter_text_fill_fs)

configure_file(src/common/core/painter/PainterShaders.h.in src/common/core/painter/PainterShaders.h)

# lua
function(build_lua_interp out_binary)
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/tools/lua_interp")
    set(lua_interp_dir "${CMAKE_CURRENT_LIST_DIR}/tools/lua_interp")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -B ${output_dir} -D CMAKE_C_COMPILER=${CMAKE_C_COMPILER} -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
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
include(scripts/solgen.cmake)
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
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/AlgineCore.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/AlgineStd.h

    src/common/internal/ConfigStrings.h

    include/common/algine/templates.h
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

# linking
if (ANDROID)
    target_link_libraries(algine assimp android log GLESv3 EGL tulz freetype lua_static)
elseif (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw opengl32 tulz freetype lua_static ${ALGINE_QT_LIBS})
elseif (UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw GL tulz freetype lua_static ${ALGINE_QT_LIBS})
endif()

# include some helpful scripts for Windows
if (WIN32)
    include(scripts/PostBuildWindows.cmake)
endif()

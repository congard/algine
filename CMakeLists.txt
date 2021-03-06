cmake_minimum_required(VERSION 3.10)
project(algine)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)
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

# built-in glsl modules
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

set(ALGINE_CORE_SOURCES
        src/common/internal/PublicObjectTools.h
        src/common/internal/SOP.h
        src/common/internal/SOPError.h
        src/common/internal/SOPConstants.h
        src/common/internal/ConfigStrings.h

        include/common/algine/templates.h
        include/common/algine/types.h
        include/common/algine/gl.h

        include/common/algine/core/transfer/Transferable.h
        include/common/algine/core/transfer/GLMTransferrer.h
        include/common/algine/core/texture/PixelData.h
        include/common/algine/core/texture/TextureCreateInfo.h
        include/common/algine/core/shader/ShaderPtr.h
        include/common/algine/core/shader/ShaderProgramPtr.h
        include/common/algine/core/texture/Texture2DPtr.h
        include/common/algine/core/texture/TextureCubePtr.h
        include/common/algine/core/texture/TextureFileInfo.h
        include/common/algine/core/RenderbufferPtr.h
        include/common/algine/core/FramebufferPtr.h
        include/common/algine/core/Ptr.h
        include/common/algine/core/PtrMaker.h
        include/common/algine/core/RawPtr.h
        include/common/algine/core/shader/ShadersInfo.h
        include/common/algine/core/DataType.h
        include/common/algine/core/EventHandler.h

        src/common/core/JsonHelper.cpp include/common/algine/core/JsonHelper.h
        src/common/core/Surface.cpp include/common/algine/core/Surface.h
        src/common/core/Content.cpp include/common/algine/core/Content.h
        src/common/core/Object.cpp include/common/algine/core/Object.h
        src/common/core/Creator.cpp include/common/algine/core/Creator.h
        src/common/core/ImageCreator.cpp include/common/algine/core/ImageCreator.h
        src/common/core/transfer/FileTransferable.cpp include/common/algine/core/transfer/FileTransferable.h
        src/common/core/OutputList.cpp include/common/algine/core/OutputList.h
        src/common/core/OutputListCreator.cpp include/common/algine/core/OutputListCreator.h
        src/common/core/FramebufferConfigTools.h
        src/common/core/Framebuffer.cpp include/common/algine/core/Framebuffer.h
        src/common/core/FramebufferCreator.cpp include/common/algine/core/FramebufferCreator.h
        src/common/core/Renderbuffer.cpp include/common/algine/core/Renderbuffer.h
        src/common/core/RenderbufferCreator.cpp include/common/algine/core/RenderbufferCreator.h
        src/common/core/buffers/Buffer.cpp include/common/algine/core/buffers/Buffer.h
        src/common/core/buffers/ArrayBuffer.cpp include/common/algine/core/buffers/ArrayBuffer.h
        src/common/core/buffers/IndexBuffer.cpp include/common/algine/core/buffers/IndexBuffer.h
        src/common/core/buffers/UniformBuffer.cpp include/common/algine/core/buffers/UniformBuffer.h
        src/common/core/buffers/BlockBufferStorage.cpp include/common/algine/core/buffers/BlockBufferStorage.h
        src/common/core/buffers/BufferWriter.cpp include/common/algine/core/buffers/BufferWriter.h
        src/common/core/buffers/BufferReader.cpp include/common/algine/core/buffers/BufferReader.h
        src/common/core/InputLayout.cpp include/common/algine/core/InputLayout.h
        src/common/core/InputAttributeDescription.cpp include/common/algine/core/InputAttributeDescription.h
        src/common/core/texture/TexturePrivateTools.h
        src/common/core/texture/Texture.cpp include/common/algine/core/texture/Texture.h
        src/common/core/texture/Texture2D.cpp include/common/algine/core/texture/Texture2D.h
        src/common/core/texture/TextureCube.cpp include/common/algine/core/texture/TextureCube.h
        src/common/core/texture/TextureTools.cpp include/common/algine/core/texture/TextureTools.h
        src/common/core/texture/TextureCreator.cpp include/common/algine/core/texture/TextureCreator.h
        src/common/core/texture/Texture2DCreator.cpp include/common/algine/core/texture/Texture2DCreator.h
        src/common/core/texture/TextureCubeCreator.cpp include/common/algine/core/texture/TextureCubeCreator.h
        src/common/core/shader/ShaderTools.cpp include/common/algine/core/shader/ShaderTools.h
        src/common/core/shader/Shader.cpp include/common/algine/core/shader/Shader.h
        src/common/core/shader/ShaderCreator.cpp include/common/algine/core/shader/ShaderCreator.h
        src/common/core/shader/ShaderDefinitionGenerator.cpp include/common/algine/core/shader/ShaderDefinitionGenerator.h
        src/common/core/shader/ShaderProgramCreator.cpp include/common/algine/core/shader/ShaderProgramCreator.h
        src/common/core/shader/ShaderProgram.cpp include/common/algine/core/shader/ShaderProgram.h
        src/common/core/shader/BaseUniformBlock.cpp include/common/algine/core/shader/BaseUniformBlock.h
        src/common/core/shader/UniformBlock.cpp include/common/algine/core/shader/UniformBlock.h
        src/common/core/debug/DebugWriter.cpp include/common/algine/core/debug/DebugWriter.h
        src/common/core/debug/ConsoleDebugWriter.cpp include/common/algine/core/debug/ConsoleDebugWriter.h
        src/common/core/debug/FileDebugWriter.cpp include/common/algine/core/debug/FileDebugWriter.h
        src/common/core/Engine.cpp include/common/algine/core/Engine.h
        src/common/core/TypeRegistry.cpp include/common/algine/core/TypeRegistry.h
        src/common/core/ImageConfigTools.h
        src/common/core/TextureConfigTools.h
        src/common/core/log/Logger.cpp include/common/algine/core/log/Logger.h
        src/common/core/log/Log.cpp include/common/algine/core/log/Log.h
        src/common/core/debug/Debug.h src/common/core/debug/Debug.cpp
        src/common/core/io/IOSystem.cpp include/common/algine/core/io/IOSystem.h
        src/common/core/io/IOStream.cpp include/common/algine/core/io/IOStream.h
        src/common/core/io/StandardIOSystem.cpp include/common/algine/core/io/StandardIOSystem.h
        src/common/core/io/StandardIOStream.cpp include/common/algine/core/io/StandardIOStream.h
        src/common/core/io/IOProvider.cpp include/common/algine/core/io/IOProvider.h
        src/common/core/IOStreamUtils.h
        src/common/core/glsl-modules/GLSLModules.h.in
        src/common/core/font/Font.cpp include/common/algine/core/font/Font.h
        src/common/core/font/FontRenderer.cpp include/common/algine/core/font/FontRenderer.h
        src/common/core/font/FontMetrics.cpp include/common/algine/core/font/FontMetrics.h
        include/common/algine/core/Rect.h
        include/common/algine/core/Point.h
        src/common/core/Color.cpp include/common/algine/core/Color.h
        src/common/core/painter/Painter.cpp include/common/algine/core/painter/Painter.h
        src/common/core/painter/Paint.cpp include/common/algine/core/painter/Paint.h
        include/common/algine/core/painter/RoundRect.h)

set(ALGINE_STD_SOURCES
        include/common/algine/std/animation/BoneSystemModuleConstants.h
        include/common/algine/constants/ShadowShader.h
        include/common/algine/constants/QuadShader.h
        include/common/algine/constants/CubemapShader.h

        include/common/algine/std/QuadRendererPtr.h
        include/common/algine/std/CubeRendererPtr.h
        include/common/algine/std/model/ShapePtr.h
        include/common/algine/std/model/Mesh.h
        include/common/algine/std/model/ModelPtr.h
        include/common/algine/std/model/InputLayoutShapeLocations.h
        include/common/algine/std/animation/BoneMatrix.h
        include/common/algine/std/animation/BoneMatrices.h

        src/common/std/assimp2glm.h
        src/common/std/model/Model.cpp include/common/algine/std/model/Model.h
        src/common/std/model/ShapeConfigTools.h
        src/common/std/model/Shape.cpp include/common/algine/std/model/Shape.h
        src/common/std/model/ShapeCreator.cpp include/common/algine/std/model/ShapeCreator.h
        src/common/std/model/InputLayoutShapeLocationsCreator.cpp include/common/algine/std/model/InputLayoutShapeLocationsCreator.h
        src/common/std/model/ModelCreator.cpp include/common/algine/std/model/ModelCreator.h
        src/common/std/Node.cpp include/common/algine/std/Node.h
        src/common/std/animation/VecAnimKey.cpp include/common/algine/std/animation/VecAnimKey.h
        src/common/std/animation/QuatAnimKey.cpp include/common/algine/std/animation/QuatAnimKey.h
        src/common/std/animation/AnimNode.cpp include/common/algine/std/animation/AnimNode.h
        src/common/std/animation/Animation.cpp include/common/algine/std/animation/Animation.h
        src/common/std/animation/Animator.cpp include/common/algine/std/animation/Animator.h
        src/common/std/animation/Bone.cpp include/common/algine/std/animation/Bone.h
        src/common/std/animation/BoneInfo.cpp include/common/algine/std/animation/BoneInfo.h
        src/common/std/animation/BonesStorage.cpp include/common/algine/std/animation/BonesStorage.h
        src/common/std/animation/AnimationBlender.cpp include/common/algine/std/animation/AnimationBlender.h
        src/common/std/animation/BoneSystemManager.cpp include/common/algine/std/animation/BoneSystemManager.h
        src/common/std/Camera.cpp include/common/algine/std/Camera.h
        src/common/std/rotator/Rotator.cpp include/common/algine/std/rotator/Rotator.h
        src/common/std/rotator/EulerRotator.cpp include/common/algine/std/rotator/EulerRotator.h
        src/common/std/rotator/FreeRotator.cpp include/common/algine/std/rotator/FreeRotator.h
        src/common/std/CubeRenderer.cpp include/common/algine/std/CubeRenderer.h
        src/common/std/QuadRenderer.cpp include/common/algine/std/QuadRenderer.h
        src/common/std/Rotatable.cpp include/common/algine/std/Rotatable.h
        src/common/std/Translatable.cpp include/common/algine/std/Translatable.h
        src/common/std/Scalable.cpp include/common/algine/std/Scalable.h
        src/common/std/AMTLMaterialManager.cpp include/common/algine/std/AMTLMaterialManager.h
        src/common/std/AMTLManager.cpp include/common/algine/std/AMTLManager.h
        src/common/std/Material.cpp include/common/algine/std/Material.h
        src/common/std/Blur.cpp include/common/algine/std/Blur.h)

set(ALGINE_EXT_SOURCES
        include/common/algine/ext/debug.h

        include/common/algine/ext/lighting/LightingConstants.h
        src/common/ext/lighting/LightingManager.cpp include/common/algine/ext/lighting/LightingManager.h
        src/common/ext/lighting/Light.cpp include/common/algine/ext/lighting/Light.h
        src/common/ext/lighting/PointLight.cpp include/common/algine/ext/lighting/PointLight.h
        src/common/ext/lighting/DirLight.cpp include/common/algine/ext/lighting/DirLight.h
        src/common/ext/lighting/PointLamp.cpp include/common/algine/ext/lighting/PointLamp.h
        src/common/ext/lighting/DirLamp.cpp include/common/algine/ext/lighting/DirLamp.h)

if (NOT ANDROID)
    set(ALGINE_DESKTOP_SOURCES
            include/desktop/algine/core/input/MouseKey.h
            include/desktop/algine/core/input/KeyboardKey.h

            src/desktop/core/input/MouseKeyConverter.h
            src/desktop/core/input/KeyboardKeyConverter.h
            src/desktop/core/window/Window.cpp include/desktop/algine/core/window/Window.h
            src/desktop/core/window/Icon.cpp include/desktop/algine/core/window/Icon.h
            src/desktop/core/window/Icon2GLFWimage.h
            src/desktop/core/window/Cursor.cpp include/desktop/algine/core/window/Cursor.h
            src/desktop/core/window/WindowEventHandler.cpp include/desktop/algine/core/window/WindowEventHandler.h)

    # Under Windows you can compile this project using GNU compiler (for example by using MinGW-w64)
    # You can use CLion or QtCreator as IDE

    set(BUILD_UTILS OFF) # glew flag

    if (BUILD_SHARED_LIBS)
        set(GLEW_LIBRARY glew)
    else()
        set(GLEW_LIBRARY glew_s)

        add_definitions(-DGLEW_STATIC)
    endif()

    add_subdirectory(contrib/glew/build/cmake)

    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)

    add_subdirectory(contrib/glfw)
else()
    set(ALGINE_ANDROID_SOURCES
            include/android/algine/Android.h

            src/android/core/Bridge.cpp src/android/core/Bridge.h
            src/android/core/Screen.cpp include/android/algine/core/Screen.h
            src/android/core/ScreenEventHandler.cpp include/android/algine/core/ScreenEventHandler.h
            src/android/core/io/AssetsIOSystem.cpp include/android/algine/core/io/AssetsIOSystem.h
            src/android/core/io/AssetsIOStream.cpp include/android/algine/core/io/AssetsIOStream.h)
endif()

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_NO_EXPORT ON)

add_subdirectory(contrib/tulz)
add_subdirectory(contrib/assimp)
add_subdirectory(contrib/freetype)

add_library(algine
        ${ALGINE_CORE_SOURCES}
        ${ALGINE_STD_SOURCES}
        ${ALGINE_EXT_SOURCES}
        ${ALGINE_DESKTOP_SOURCES}
        ${ALGINE_ANDROID_SOURCES})

set_target_properties(algine PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(algine PRIVATE src/common)
target_include_directories(algine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-modules)
target_include_directories(algine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/glsl-shaders)
target_include_directories(algine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src/common/core/painter)

target_include_directories(algine PRIVATE contrib/assimp/include)
target_include_directories(algine PRIVATE contrib/freetype/include)

target_include_directories(algine PUBLIC contrib/tulz/include)
target_include_directories(algine PUBLIC contrib/glm)
target_include_directories(algine PUBLIC include/common)
target_include_directories(algine PUBLIC contrib)

# TODO: ^^^ maybe move one header libs to, for example, libname/include/libname/file.hpp instead of libname/file.hpp

if (NOT ANDROID)
    target_include_directories(algine PRIVATE src/desktop)

    target_include_directories(algine PRIVATE contrib/glfw/include)

    target_include_directories(algine PUBLIC contrib/glew/include) # TODO: mark PRIVATE in the future (see Texture.h)
    target_include_directories(algine PUBLIC include/desktop)
else()
    target_include_directories(algine PUBLIC contrib/tulz/android/include)
    target_include_directories(algine PUBLIC include/android)
endif()

if (MSVC OR ANDROID)
    target_include_directories(algine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/include)
endif()

# linking
if (ANDROID)
    target_link_libraries(algine assimp android log GLESv3 EGL tulz freetype)
elseif (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw opengl32 tulz freetype)
elseif (UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw GL tulz freetype)
endif()

# include some helpful scripts for Windows
if (WIN32)
    include(cmake/PostBuildWindows.cmake)
endif()

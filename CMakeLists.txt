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

set(ALGINE_CORE_SOURCES
        src/common/internal/PublicObjectTools.h
        src/common/internal/SOP.h
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
        include/common/algine/core/RenderbufferPtr.h
        include/common/algine/core/FramebufferPtr.h
        include/common/algine/core/Ptr.h
        include/common/algine/core/PtrMaker.h
        include/common/algine/core/RawPtr.h
        include/common/algine/core/shader/ShadersInfo.h
        include/common/algine/core/DataType.h
        include/common/algine/core/EventHandler.h

        src/common/core/JsonHelper.cpp include/common/algine/core/JsonHelper.h
        src/common/core/Object.cpp include/common/algine/core/Object.h
        src/common/core/ManagerBase.cpp include/common/algine/core/ManagerBase.h
        src/common/core/ImageManagerBase.cpp include/common/algine/core/ImageManagerBase.h
        src/common/core/transfer/FileTransferable.cpp include/common/algine/core/transfer/FileTransferable.h
        src/common/core/OutputList.cpp include/common/algine/core/OutputList.h
        src/common/core/OutputListManager.cpp include/common/algine/core/OutputListManager.h
        src/common/core/FramebufferConfigTools.h
        src/common/core/Framebuffer.cpp include/common/algine/core/Framebuffer.h
        src/common/core/FramebufferManager.cpp include/common/algine/core/FramebufferManager.h
        src/common/core/Renderbuffer.cpp include/common/algine/core/Renderbuffer.h
        src/common/core/RenderbufferManager.cpp include/common/algine/core/RenderbufferManager.h
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
        src/common/core/texture/TextureManager.cpp include/common/algine/core/texture/TextureManager.h
        src/common/core/texture/Texture2DManager.cpp include/common/algine/core/texture/Texture2DManager.h
        src/common/core/texture/TextureCubeManager.cpp include/common/algine/core/texture/TextureCubeManager.h
        src/common/core/shader/ShaderTools.cpp include/common/algine/core/shader/ShaderTools.h
        src/common/core/shader/Shader.cpp include/common/algine/core/shader/Shader.h
        src/common/core/shader/ShaderManager.cpp include/common/algine/core/shader/ShaderManager.h
        src/common/core/shader/ShaderDefinitionManager.cpp include/common/algine/core/shader/ShaderDefinitionManager.h
        src/common/core/shader/ShaderProgramManager.cpp include/common/algine/core/shader/ShaderProgramManager.h
        src/common/core/shader/ShaderProgram.cpp include/common/algine/core/shader/ShaderProgram.h
        src/common/core/shader/BaseUniformBlock.cpp include/common/algine/core/shader/BaseUniformBlock.h
        src/common/core/shader/UniformBlock.cpp include/common/algine/core/shader/UniformBlock.h
        src/common/core/Engine.cpp include/common/algine/core/Engine.h
        src/common/core/TypeRegistry.cpp include/common/algine/core/TypeRegistry.h
        src/common/core/ImageConfigTools.h
        src/common/core/TextureConfigTools.h
        src/common/core/log/Logger.cpp include/common/algine/core/log/Logger.h
        src/common/core/log/Log.cpp include/common/algine/core/log/Log.h)

set(ALGINE_STD_SOURCES
        include/common/algine/constants/BoneSystem.h
        include/common/algine/constants/ShadowShader.h
        include/common/algine/constants/QuadShader.h
        include/common/algine/constants/CubemapShader.h
        include/common/algine/constants/Lighting.h
        include/common/algine/constants/Material.h
        include/common/algine/constants/NormalMapping.h

        include/common/algine/std/QuadRendererPtr.h
        include/common/algine/std/CubeRendererPtr.h
        include/common/algine/std/model/ShapePtr.h
        include/common/algine/std/model/Mesh.h
        include/common/algine/std/model/ModelPtr.h
        include/common/algine/std/model/InputLayoutShapeLocations.h
        include/common/algine/std/Material.h
        include/common/algine/std/animation/BoneMatrix.h
        include/common/algine/std/animation/BoneMatrices.h

        src/common/std/assimp2glm.h
        src/common/std/model/Model.cpp include/common/algine/std/model/Model.h
        src/common/std/model/ShapeConfigTools.h
        src/common/std/model/Shape.cpp include/common/algine/std/model/Shape.h
        src/common/std/model/ShapeManager.cpp include/common/algine/std/model/ShapeManager.h
        src/common/std/model/InputLayoutShapeLocationsManager.cpp include/common/algine/std/model/InputLayoutShapeLocationsManager.h
        src/common/std/model/ModelManager.cpp include/common/algine/std/model/ModelManager.h
        src/common/std/Node.cpp include/common/algine/std/Node.h
        src/common/std/lighting/LightingManager.cpp include/common/algine/std/lighting/LightingManager.h
        src/common/std/lighting/Light.cpp include/common/algine/std/lighting/Light.h
        src/common/std/lighting/PointLight.cpp include/common/algine/std/lighting/PointLight.h
        src/common/std/lighting/DirLight.cpp include/common/algine/std/lighting/DirLight.h
        src/common/std/lighting/PointLamp.cpp include/common/algine/std/lighting/PointLamp.h
        src/common/std/lighting/DirLamp.cpp include/common/algine/std/lighting/DirLamp.h
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
        src/common/std/camera/Camera.cpp include/common/algine/std/camera/Camera.h
        src/common/std/rotator/Rotator.cpp include/common/algine/std/rotator/Rotator.h
        src/common/std/rotator/EulerRotator.cpp include/common/algine/std/rotator/EulerRotator.h
        src/common/std/rotator/FreeRotator.cpp include/common/algine/std/rotator/FreeRotator.h
        src/common/std/CubeRenderer.cpp include/common/algine/std/CubeRenderer.h
        src/common/std/QuadRenderer.cpp include/common/algine/std/QuadRenderer.h
        src/common/std/Rotatable.cpp include/common/algine/std/Rotatable.h
        src/common/std/Translatable.cpp include/common/algine/std/Translatable.h
        src/common/std/Scalable.cpp include/common/algine/std/Scalable.h
        src/common/std/AMTLMaterialManager.cpp include/common/algine/std/AMTLMaterialManager.h
        src/common/std/AMTLManager.cpp include/common/algine/std/AMTLManager.h)

set(ALGINE_EXT_SOURCES
        include/common/algine/ext/constants/BlurShader.h
        include/common/algine/ext/constants/SSRShader.h
        include/common/algine/ext/constants/BlendBloom.h
        include/common/algine/ext/constants/BloomSearchShader.h
        include/common/algine/ext/constants/COCShader.h
        include/common/algine/ext/constants/BlendDOF.h

        include/common/algine/ext/debug.h

        src/common/ext/Blur.cpp include/common/algine/ext/Blur.h)

if (NOT ANDROID)
    set(ALGINE_DESKTOP_SOURCES
            include/desktop/algine/core/input/MouseKey.h
            include/desktop/algine/core/input/KeyboardKey.h
            include/desktop/algine/core/window/DebugWriter.h

            src/desktop/core/input/MouseKeyConverter.h
            src/desktop/core/input/KeyboardKeyConverter.h
            src/desktop/core/window/Window.cpp include/desktop/algine/core/window/Window.h
            src/desktop/core/window/Content.cpp include/desktop/algine/core/window/Content.h
            src/desktop/core/window/ConsoleDebugWriter.cpp include/desktop/algine/core/window/ConsoleDebugWriter.h
            src/desktop/core/window/FileDebugWriter.cpp include/desktop/algine/core/window/FileDebugWriter.h
            src/desktop/core/window/Icon.cpp include/desktop/algine/core/window/Icon.h
            src/desktop/core/window/Icon2GLFWimage.h
            src/desktop/core/window/Cursor.cpp include/desktop/algine/core/window/Cursor.h)

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
endif()

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)

add_subdirectory(contrib/tulz)
add_subdirectory(contrib/assimp)

include_directories(contrib)

add_library(algine
        ${ALGINE_CORE_SOURCES}
        ${ALGINE_STD_SOURCES}
        ${ALGINE_EXT_SOURCES}
        ${ALGINE_DESKTOP_SOURCES}
        ${ALGINE_ANDROID_SOURCES})

set_target_properties(algine PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(algine PRIVATE src/common)

target_include_directories(algine PRIVATE contrib/assimp/include)

target_include_directories(algine PUBLIC contrib/tulz/include)
target_include_directories(algine PUBLIC contrib/glm)
target_include_directories(algine PUBLIC include/common)

if (NOT ANDROID)
    target_include_directories(algine PRIVATE src/desktop)

    target_include_directories(algine PRIVATE contrib/glfw/include)

    target_include_directories(algine PUBLIC contrib/glew/include) # TODO: mark PRIVATE in the future (see Texture.h)
    target_include_directories(algine PUBLIC include/desktop)
endif()

if (MSVC)
    target_include_directories(algine PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/include)
endif()

# linking
if (ANDROID)
    target_link_libraries(algine assimp android log GLESv3 EGL tulz)
elseif (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw opengl32 tulz)
elseif (UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw GL tulz)
endif()

# include some helpful scripts for Windows
if (WIN32)
    include(cmake/PostBuildWindows.cmake)
endif()

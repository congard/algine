cmake_minimum_required(VERSION 3.14)
project(algine)

set(CMAKE_CXX_STANDARD 14)

# Under Windows you can compile this project using GNU compiler (for example by using MinGW-w64)
# You can use CLion or QtCreator as IDE

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Algine debug mode - adding -g option")
    add_compile_options("-g")

    if (NOT DEFINED ALGINE_SECURE_OPERATIONS)
        message(STATUS "Algine SOP set to ON")
        set(ALGINE_SECURE_OPERATIONS ON)
    endif()
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Algine release mode")

    # should inline functions like bind(), unbind() etc
    add_compile_options("-flto")

    if (NOT DEFINED ALGINE_SECURE_OPERATIONS)
        message(STATUS "Algine SOP set to OFF")
        set(ALGINE_SECURE_OPERATIONS OFF)
    endif()
else()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}\nBut you can choose only Debug or Release build type")
endif()

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(BUILD_UTILS OFF) # glew flag

if (ALGINE_LINK_LIBS_STATICALLY)
    add_definitions(-DGLEW_STATIC)
    set(BUILD_SHARED_LIBS OFF)
else()
    set(BUILD_SHARED_LIBS ON)
endif()

if (ALGINE_USE_SYSTEM_ASSIMP)
    find_package(assimp REQUIRED)
    message(STATUS "Algine: use system Assimp")
else()
    add_subdirectory(contrib/assimp)
    include_directories(contrib/assimp/include)
endif()
if (ALGINE_USE_SYSTEM_GLEW)
    find_package(GLEW REQUIRED)
    set(GLEW_LIBRARY GLEW)
    message(STATUS "Algine: use system GLEW")
else()
    add_subdirectory(contrib/glew/build/cmake)
    include_directories(contrib/glew/include)
    if (ALGINE_LINK_LIBS_STATICALLY)
        set(GLEW_LIBRARY glew_s)
    else()
        set(GLEW_LIBRARY glew)
    endif()
endif()

add_subdirectory(contrib/tulz)
include_directories(contrib/tulz/include)
include_directories(contrib/glm)
include_directories(contrib)
include_directories(include)

if (ALGINE_SECURE_OPERATIONS)
    if (NOT DEFINED ALGINE_SOP_LEVEL)
        set(ALGINE_SOP_LEVEL 0)
    endif()

    add_compile_definitions(ALGINE_SECURE_OPERATIONS)
    add_compile_definitions(ALGINE_SOP_LEVEL=${ALGINE_SOP_LEVEL})
    message(STATUS "Algine SOP level is ${ALGINE_SOP_LEVEL}")
endif()

add_library(algine
        include/algine/types.h
        include/algine/templates.h
        include/algine/constants/AMTL.h
        include/algine/constants/BoneSystem.h
        include/algine/constants/ShadowShader.h
        include/algine/constants/QuadShader.h
        include/algine/constants/CubemapShader.h
        include/algine/constants/Lighting.h
        include/algine/constants/Material.h
        include/algine/constants/NormalMapping.h
        include/algine/ext/constants/BlurShader.h
        include/algine/ext/constants/SSRShader.h
        include/algine/ext/constants/BlendBloom.h
        include/algine/ext/constants/BloomSearchShader.h
        include/algine/ext/constants/COCShader.h
        include/algine/ext/constants/BlendDOF.h
        include/algine/core/transfer/GLMTransferrer.h
        include/algine/core/texture/PixelData.h
        include/algine/core/texture/TextureCreateInfo.h
        include/algine/core/shader/ShadersInfo.h
        include/algine/std/model/Mesh.h
        include/algine/std/model/InputLayoutShapeLocations.h

        src/core/Object.cpp include/algine/core/Object.h
        src/core/transfer/Transferable.cpp include/algine/core/transfer/Transferable.h
        src/core/Framebuffer.cpp include/algine/core/Framebuffer.h
        src/core/Renderbuffer.cpp include/algine/core/Renderbuffer.h
        src/core/buffers/Buffer.cpp include/algine/core/buffers/Buffer.h
        src/core/buffers/ArrayBuffer.cpp include/algine/core/buffers/ArrayBuffer.h
        src/core/buffers/IndexBuffer.cpp include/algine/core/buffers/IndexBuffer.h
        src/core/buffers/UniformBuffer.cpp include/algine/core/buffers/UniformBuffer.h
        src/core/buffers/BlockBufferStorage.cpp include/algine/core/buffers/BlockBufferStorage.h
        src/core/buffers/BufferWriter.cpp include/algine/core/buffers/BufferWriter.h
        src/core/buffers/BufferReader.cpp include/algine/core/buffers/BufferReader.h
        src/core/InputLayout.cpp include/algine/core/InputLayout.h
        src/core/InputAttributeDescription.cpp include/algine/core/InputAttributeDescription.h
        src/core/texture/Texture.cpp include/algine/core/texture/Texture.h
        src/core/texture/Texture2D.cpp include/algine/core/texture/Texture2D.h
        src/core/texture/TextureCube.cpp include/algine/core/texture/TextureCube.h
        src/core/texture/TextureTools.cpp include/algine/core/texture/TextureTools.h
        src/core/shader/ShaderTools.cpp include/algine/core/shader/ShaderTools.h
        src/core/shader/Shader.cpp include/algine/core/shader/Shader.h
        src/core/shader/ShaderManager.cpp include/algine/core/shader/ShaderManager.h
        src/core/shader/ShaderDefinitionManager.cpp include/algine/core/shader/ShaderDefinitionManager.h
        src/core/shader/ShaderProgramManager.cpp include/algine/core/shader/ShaderProgramManager.h
        src/core/shader/ShaderProgramManagerLegacy.cpp include/algine/core/shader/ShaderProgramManagerLegacy.h
        src/core/shader/ShaderProgram.cpp include/algine/core/shader/ShaderProgram.h
        src/core/shader/BaseUniformBlock.cpp include/algine/core/shader/BaseUniformBlock.h
        src/core/shader/UniformBlock.cpp include/algine/core/shader/UniformBlock.h
        src/core/Engine.cpp include/algine/core/Engine.h
        src/core/SOP.h src/core/SOPConstants.h
        src/core/NativeConstants.h

        include/algine/std/Material.h
        src/std/assimp2glm.h
        src/core/JsonHelper.cpp include/algine/core/JsonHelper.h
        src/std/model/Model.cpp include/algine/std/model/Model.h
        src/std/model/Shape.cpp include/algine/std/model/Shape.h
        src/std/model/ShapeLoader.cpp include/algine/std/model/ShapeLoader.h
        src/std/Node.cpp include/algine/std/Node.h
        src/std/AMTLLoader.cpp include/algine/std/AMTLLoader.h
        src/std/lighting/LightingManager.cpp include/algine/std/lighting/LightingManager.h
        src/std/lighting/Light.cpp include/algine/std/lighting/Light.h
        src/std/lighting/PointLight.cpp include/algine/std/lighting/PointLight.h
        src/std/lighting/DirLight.cpp include/algine/std/lighting/DirLight.h
        src/std/lighting/PointLamp.cpp include/algine/std/lighting/PointLamp.h
        src/std/lighting/DirLamp.cpp include/algine/std/lighting/DirLamp.h
        src/std/animation/VecAnimKey.cpp include/algine/std/animation/VecAnimKey.h
        src/std/animation/QuatAnimKey.cpp include/algine/std/animation/QuatAnimKey.h
        src/std/animation/AnimNode.cpp include/algine/std/animation/AnimNode.h
        src/std/animation/Animation.cpp include/algine/std/animation/Animation.h
        src/std/animation/Animator.cpp include/algine/std/animation/Animator.h
        src/std/animation/Bone.cpp include/algine/std/animation/Bone.h
        src/std/animation/BoneInfo.cpp include/algine/std/animation/BoneInfo.h
        src/std/animation/BonesStorage.cpp include/algine/std/animation/BonesStorage.h
        src/std/animation/AnimationBlender.cpp include/algine/std/animation/AnimationBlender.h
        src/std/animation/BoneSystemManager.cpp include/algine/std/animation/BoneSystemManager.h
        src/std/camera/Camera.cpp include/algine/std/camera/Camera.h
        src/std/camera/BaseCameraController.cpp include/algine/std/camera/BaseCameraController.h
        src/std/camera/FreeCameraController.cpp include/algine/std/camera/FreeCameraController.h
        src/std/camera/FPSCameraController.cpp include/algine/std/camera/FPSCameraController.h
        src/std/rotator/Rotator.cpp include/algine/std/rotator/Rotator.h
        src/std/rotator/EulerRotator.cpp include/algine/std/rotator/EulerRotator.h
        src/std/rotator/FreeRotator.cpp include/algine/std/rotator/FreeRotator.h
        src/std/CubeRenderer.cpp include/algine/std/CubeRenderer.h
        src/std/QuadRenderer.cpp include/algine/std/QuadRenderer.h
        src/std/Rotatable.cpp include/algine/std/Rotatable.h
        src/std/Translatable.cpp include/algine/std/Translatable.h
        src/std/Scalable.cpp include/algine/std/Scalable.h

        src/ext/Blur.cpp include/algine/ext/Blur.h
        src/ext/event/MouseEventListener.cpp include/algine/ext/event/MouseEventListener.h
        src/ext/event/MouseEvent.cpp include/algine/ext/event/MouseEvent.h include/algine/ext/event/MouseButton.h
        src/ext/debug.cpp include/algine/ext/debug.h)

# linking
if (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} opengl32 tulz)
elseif(UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} GL tulz)
    if (ALGINE_USE_SYSTEM_ASSIMP)
        target_link_directories(algine
                PUBLIC /usr/local/lib)
    endif()
endif()
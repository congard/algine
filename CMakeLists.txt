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
        include/algine/constants.h
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
        src/core/Framebuffer.cpp include/algine/core/Framebuffer.h
        src/material.cpp include/algine/material.h
        src/math.cpp include/algine/math.h
        src/model.cpp include/algine/model.h
        src/node.cpp include/algine/node.h
        src/core/Renderbuffer.cpp include/algine/core/Renderbuffer.h
        src/camera.cpp include/algine/camera.h
        src/event.cpp include/algine/event.h
        src/object3d.cpp include/algine/object3d.h
        src/rotator.cpp include/algine/rotator.h
        src/debug.cpp include/algine/debug.h
        src/AMTLLoader.cpp include/algine/AMTLLoader.h
        src/core/buffers/Buffer.cpp include/algine/core/buffers/Buffer.h
        src/core/buffers/ArrayBuffer.cpp include/algine/core/buffers/ArrayBuffer.h
        src/core/buffers/IndexBuffer.cpp include/algine/core/buffers/IndexBuffer.h
        src/core/InputLayout.cpp include/algine/core/InputLayout.h
        src/core/InputAttributeDescription.cpp include/algine/core/InputAttributeDescription.h
        src/CubeRenderer.cpp include/algine/CubeRenderer.h
        src/QuadRenderer.cpp include/algine/QuadRenderer.h
        src/ext/Blur.cpp include/algine/ext/Blur.h
        src/lighting/Manager.cpp include/algine/lighting/Manager.h
        src/lighting/Transmitter.cpp include/algine/lighting/Transmitter.h
        src/lighting/Light.cpp include/algine/lighting/Light.h
        src/lighting/PointLight.cpp include/algine/lighting/PointLight.h
        src/lighting/DirLight.cpp include/algine/lighting/DirLight.h
        src/lighting/PointLamp.cpp include/algine/lighting/PointLamp.h
        src/lighting/DirLamp.cpp include/algine/lighting/DirLamp.h
        include/algine/core/texture/PixelData.h
        src/core/texture/Texture.cpp include/algine/core/texture/Texture.h
        include/algine/core/texture/TextureCreateInfo.h
        src/core/texture/Texture2D.cpp include/algine/core/texture/Texture2D.h
        src/core/texture/TextureCube.cpp include/algine/core/texture/TextureCube.h
        src/core/texture/TextureTools.cpp include/algine/core/texture/TextureTools.h
        src/animation/VecAnimKey.cpp include/algine/animation/VecAnimKey.h
        src/animation/QuatAnimKey.cpp include/algine/animation/QuatAnimKey.h
        src/animation/AnimNode.cpp include/algine/animation/AnimNode.h
        src/animation/Animation.cpp include/algine/animation/Animation.h
        src/animation/AnimShape.cpp include/algine/animation/AnimShape.h
        src/animation/Animator.cpp include/algine/animation/Animator.h
        src/animation/Bone.cpp include/algine/animation/Bone.h
        src/animation/BoneInfo.cpp include/algine/animation/BoneInfo.h
        include/algine/core/shader/ShadersInfo.h
        src/core/shader/ShaderTools.cpp include/algine/core/shader/ShaderTools.h
        src/core/shader/Shader.cpp include/algine/core/shader/Shader.h
        src/core/shader/ShaderManager.cpp include/algine/core/shader/ShaderManager.h
        src/core/shader/ShaderProgram.cpp include/algine/core/shader/ShaderProgram.h
        src/core/Engine.cpp include/algine/core/Engine.h
        src/core/SOP.h src/core/SOPConstants.h)

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
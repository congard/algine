cmake_minimum_required(VERSION 3.14)
project(algine)

set(CMAKE_CXX_STANDARD 14)

# note: compiling on Windows only tested under MinGW-w64
# most likely it will not work with Microsoft Visual Studio
# for working use CLion, QtCreator etc instead

if (WIN32)
    set(LIBS_DIR "C:\\Program\ Files\ (x86)\\")

    set(GLEW_INCLUDE_DIR "${LIBS_DIR}glew\\include")
    set(GLEW_LIBRARY "${LIBS_DIR}glew\\bin")

    set(glfw3_DIR "${LIBS_DIR}GLFW\\lib\\cmake\\glfw3")
    set(GLFW3_INCLUDE_DIRS "${LIBS_DIR}GLFW\\include")
    set(GLFW3_LIBRARIES "${LIBS_DIR}GLFW\\lib")

    set(GLM_INCLUDE_DIRS "${LIBS_DIR}glm\\include")

    set(ASSIMP_LIBS "${LIBS_DIR}Assimp\\bin")
endif()

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

if (WIN32)
    include_directories(${GLFW3_INCLUDE_DIRS})
    include_directories(${GLEW_INCLUDE_DIRS})
    include_directories(${ASSIMP_INCLUDE_DIRS})
    include_directories(${GLM_INCLUDE_DIRS})

    link_libraries(assimp glew32 glfw3 opengl32)
    link_directories(${GLFW3_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBS})
elseif (UNIX)
    link_libraries(glfw GLEW GLU GL assimp pthread)
    link_directories(/usr/local/lib)
endif()

include_directories(contrib)
include_directories(contrib/nlohmann)
include_directories(contrib/stb)
include_directories(include)
include_directories(include/algine)

add_executable(algine
        contrib/nlohmann/json.hpp
        contrib/stb/stb_image.h
        contrib/stb/stb_image_write.h
        include/algine/algine.h
        include/algine/algine_renderer.h
        include/algine/algine_structs.h
        include/algine/animation.h
        include/algine/bone.h
        include/algine/constants.h
        include/algine/core_utils.h
        include/algine/framebuffer.h
        include/algine/io.h
        include/algine/light.h
        include/algine/material.h
        include/algine/math.h
        include/algine/model.h
        include/algine/node.h
        include/algine/renderbuffer.h
        include/algine/sconstants.h
        include/algine/shader_compiler.h
        include/algine/shader_program.h
        include/algine/texture.h
        include/algine/types.h
        include/algine/camera.h
        include/algine/event.h
        src/algine_renderer.cpp
        src/animation.cpp
        src/bone.cpp
        src/core_utils.cpp
        src/framebuffer.cpp
        src/io.cpp
        src/light.cpp
        src/main.cpp
        src/material.cpp
        src/math.cpp
        src/model.cpp
        src/node.cpp
        src/renderbuffer.cpp
        src/shader_compiler.cpp
        src/shader_program.cpp
        src/texture.cpp
        src/camera.cpp
        src/event.cpp)

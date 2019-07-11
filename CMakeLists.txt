cmake_minimum_required(VERSION 3.14)
project(algine)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)

if (UNIX)
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
        include/algine/unsconstants.h
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
        src/texture.cpp)
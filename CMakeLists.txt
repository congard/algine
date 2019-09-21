cmake_minimum_required(VERSION 3.14)
project(algine)

set(CMAKE_CXX_STANDARD 14)

# Under Windows you can compile this project using GNU compiler (for example by using MinGW-w64)
# You can use CLion or QtCreator as IDE

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Algine debug mode - adding -g option")
    add_compile_options("-g")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Algine release mode")
else()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}\nBut you can choose only Debug or Release build type")
endif()

# adding / finding libs
if (WIN32)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    set(ASSIMP_BUILD_TESTS OFF)

    if (ALGINE_LINK_LIBS_STATICALLY)
        add_definitions(-DGLEW_STATIC)
        set(BUILD_SHARED_LIBS OFF)
    else()
        set(BUILD_SHARED_LIBS ON)
    endif()

    add_subdirectory(contrib/assimp)
    add_subdirectory(contrib/glfw)
    add_subdirectory(contrib/glew/build/cmake)
    include_directories(contrib/glew/include)
    include_directories(contrib/glfw/include)
    include_directories(contrib/assimp/include)
elseif (UNIX)
    find_package(GLEW REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glfw3 REQUIRED)
endif()

include_directories(contrib/glm)
include_directories(contrib)
include_directories(include)

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
        include/algine/object3d.h
        include/algine/rotator.h
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
        src/event.cpp
        src/object3d.cpp
        src/rotator.cpp)

# linking
if (WIN32)
    if (ALGINE_LINK_LIBS_STATICALLY)
        target_link_libraries(algine
                assimp
                glew_s
                glfw
                opengl32
                pthread)
    else()
        target_link_libraries(algine
                assimp
                glew
                glfw
                opengl32
                pthread)

        if (CMAKE_BUILD_TYPE MATCHES Debug)
            add_custom_command(TARGET algine POST_BUILD
                    COMMENT "Creating symlinks to libs..."
                    # assimp
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/code/libassimpd.dll ${CMAKE_CURRENT_BINARY_DIR}/libassimpd.dll
                    # glew
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/bin/glew32d.dll ${CMAKE_CURRENT_BINARY_DIR}/glew32d.dll
                    # glfw
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/glfw/src/glfw3d.dll ${CMAKE_CURRENT_BINARY_DIR}/glfw3d.dll)
        else() # if not Debug or Release CMake must throw an error in the very beginning
            add_custom_command(TARGET algine POST_BUILD
                    COMMENT "Creating symlinks to libs..."
                    # assimp
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/code/libassimp.dll ${CMAKE_CURRENT_BINARY_DIR}/libassimp.dll
                    # glew
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/bin/glew32.dll ${CMAKE_CURRENT_BINARY_DIR}/glew32.dll
                    # glfw
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/glfw/src/glfw3.dll ${CMAKE_CURRENT_BINARY_DIR}/glfw3.dll)
        endif()
    endif()
elseif(UNIX)
    target_link_libraries(algine
            glfw
            GLEW
            GL
            assimp
            pthread)
    target_link_directories(algine
            PUBLIC /usr/local/lib)
endif()
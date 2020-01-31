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

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
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
if (ALGINE_USE_SYSTEM_GLFW)
    find_package(glfw3 REQUIRED)
    message(STATUS "Algine: use system GLFW")
else()
    add_subdirectory(contrib/glfw)
    include_directories(contrib/glfw/include)
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

add_executable(algine
        include/algine/constants.h
        include/algine/types.h
        include/algine/templates.h
        src/main.cpp
        src/algine_renderer.cpp include/algine/algine_renderer.h
        src/animation.cpp include/algine/animation.h
        src/bone.cpp include/algine/bone.h
        src/gputils.cpp include/algine/gputils.h
        src/framebuffer.cpp include/algine/framebuffer.h
        src/light.cpp include/algine/light.h
        src/material.cpp include/algine/material.h
        src/math.cpp include/algine/math.h
        src/model.cpp include/algine/model.h
        src/node.cpp include/algine/node.h
        src/renderbuffer.cpp include/algine/renderbuffer.h
        src/shader.cpp include/algine/shader.h
        src/texture.cpp include/algine/texture.h
        src/camera.cpp include/algine/camera.h
        src/event.cpp include/algine/event.h
        src/object3d.cpp include/algine/object3d.h
        src/rotator.cpp include/algine/rotator.h
        src/debug.cpp include/algine/debug.h
        src/AMTLLoader.cpp include/algine/AMTLLoader.h
        src/Buffer.cpp include/algine/Buffer.h
        src/ArrayBuffer.cpp include/algine/ArrayBuffer.h
        src/ElementArrayBuffer.cpp include/algine/ElementArrayBuffer.h)

# linking
if (WIN32)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw opengl32 pthread tulz)
    if (NOT ALGINE_LINK_LIBS_STATICALLY)
        if (CMAKE_BUILD_TYPE MATCHES Debug)
            add_custom_command(TARGET algine POST_BUILD
                    COMMENT "Creating symlinks to libs..."
                    # assimp
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/code/libassimpd.dll ${CMAKE_CURRENT_BINARY_DIR}/libassimpd.dll
                    # glew
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/bin/glew32d.dll ${CMAKE_CURRENT_BINARY_DIR}/glew32d.dll
                    # glfw
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/glfw/src/glfw3d.dll ${CMAKE_CURRENT_BINARY_DIR}/glfw3d.dll
                    # tulz
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/tulz/libtulzd.dll ${CMAKE_CURRENT_BINARY_DIR}/libtulzd.dll)
        else() # if not Debug or Release CMake must throw an error in the very beginning
            add_custom_command(TARGET algine POST_BUILD
                    COMMENT "Creating symlinks to libs..."
                    # assimp
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/assimp/code/libassimp.dll ${CMAKE_CURRENT_BINARY_DIR}/libassimp.dll
                    # glew
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/bin/glew32.dll ${CMAKE_CURRENT_BINARY_DIR}/glew32.dll
                    # glfw
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/glfw/src/glfw3.dll ${CMAKE_CURRENT_BINARY_DIR}/glfw3.dll
                    # tulz
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/contrib/tulz/libtulz.dll ${CMAKE_CURRENT_BINARY_DIR}/libtulz.dll)
        endif()
    endif()
elseif(UNIX)
    target_link_libraries(algine assimp ${GLEW_LIBRARY} glfw GL pthread tulz)
    if (ALGINE_USE_SYSTEM_ASSIMP)
        target_link_directories(algine
                PUBLIC /usr/local/lib)
    endif()
endif()
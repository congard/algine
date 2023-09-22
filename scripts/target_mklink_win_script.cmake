# Do not include this file to your CMakeLists
# include PostBuildWindows.cmake instead

# Current binary directory of the current target
if (NOT DEFINED CURRENT_BINARY_DIR)
    message(FATAL_ERROR "CURRENT_BINARY_DIR is not defined")
endif()

# Directory where the algine binaries are located
if (NOT DEFINED ALGINE_BINARY_DIR)
    message(FATAL_ERROR "ALGINE_BINARY_DIR is not defined")
endif()

# Directory where all other binaries are located
if (NOT DEFINED BINARY_DIR)
    message(FATAL_ERROR "BINARY_DIR is not defined")
endif()

if (NOT DEFINED ALGINE_DEPS_BUILD_DIR_NAME)
    message(FATAL_ERROR "ALGINE_DEPS_BUILD_DIR_NAME is not defined")
endif()

function(dll_symlink dll_path)
    file(GLOB DLL_LIB_PATH ${dll_path})

    if (DLL_LIB_PATH STREQUAL "")
        message(STATUS "File ${dll_path} does not exist. Skipping")
        return()
    endif()

    get_filename_component(DLL_LIB_NAME ${DLL_LIB_PATH} NAME)
    get_filename_component(DLL_LIB_REAL_PATH ${DLL_LIB_PATH} REALPATH)
    get_filename_component(DLL_LIB_REAL_PATH_LINK ${CURRENT_BINARY_DIR}/${DLL_LIB_NAME} REALPATH)

    if (EXISTS ${DLL_LIB_REAL_PATH_LINK})
        return()
    endif()

    message(STATUS "${DLL_LIB_REAL_PATH} => ${DLL_LIB_REAL_PATH_LINK}")

    string(REPLACE "/" "\\" DLL_LIB_REAL_PATH "${DLL_LIB_REAL_PATH}")
    string(REPLACE "/" "\\" DLL_LIB_REAL_PATH_LINK "${DLL_LIB_REAL_PATH_LINK}")

    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${DLL_LIB_REAL_PATH}"
        "${DLL_LIB_REAL_PATH_LINK}")
endfunction()

dll_symlink("${ALGINE_BINARY_DIR}/*algine*.dll")
dll_symlink("${ALGINE_BINARY_DIR}/${ALGINE_DEPS_BUILD_DIR_NAME}/assimp/bin/*assimp*.dll")
dll_symlink("${ALGINE_BINARY_DIR}/${ALGINE_DEPS_BUILD_DIR_NAME}/tulz/*tulz*.dll")
dll_symlink("${ALGINE_BINARY_DIR}/${ALGINE_DEPS_BUILD_DIR_NAME}/glfw/src/*glfw3*.dll")
dll_symlink("${ALGINE_BINARY_DIR}/${ALGINE_DEPS_BUILD_DIR_NAME}/freetype/*freetype*.dll")
dll_symlink("${BINARY_DIR}/*glew32*.dll")

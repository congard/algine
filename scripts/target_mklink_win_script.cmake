# Do not include this file to your CMakeLists
# include PostBuildWindows.cmake instead

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

    execute_process(COMMAND cmd /C "mklink ${DLL_LIB_REAL_PATH_LINK} ${DLL_LIB_REAL_PATH} > NUL")
endfunction()

set(ALGINE_BIN_PATH "${CURRENT_BINARY_DIR}/${PATH_TO_ALGINE}")

dll_symlink("${ALGINE_BIN_PATH}/*algine*.dll")
dll_symlink("${ALGINE_BIN_PATH}/libs/assimp/code/*assimp*.dll")
dll_symlink("${ALGINE_BIN_PATH}/libs/tulz/*tulz*.dll")
dll_symlink("${ALGINE_BIN_PATH}/libs/glfw/src/*glfw3*.dll")
dll_symlink("${ALGINE_BIN_PATH}/libs/freetype/*freetype*.dll")
dll_symlink("${CURRENT_BINARY_DIR}/bin/*glew32*.dll")

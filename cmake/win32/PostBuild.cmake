# this file included in CMakeLists.txt

if (NOT WIN32)
    message(FATAL_ERROR "You can include PostBuildWindows.cmake only on Windows")
endif()

if (NOT DEFINED ALGINE_SOURCE_DIR)
    message(FATAL_ERROR "ALGINE_SOURCE_DIR is not defined")
endif()

# Symlink DLLs to the `${CMAKE_CURRENT_BINARY_DIR}` directory.
# You must call this function on Windows.
#
# Available mono-valued arguments:
# - `TARGET` The name of the target to create symlinks for.
# - `ALGINE_BINARY_DIR` The algine binary directory.
#   By default, `${CMAKE_CURRENT_BINARY_DIR}/algine` will be used.
# - `BINARY_DIR` The directory of binaries.
#   By default, `${CMAKE_CURRENT_BINARY_DIR}/bin` will be used.
function(algine_target_mklink)
    cmake_parse_arguments(
        PARSED_ARG # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "TARGET;ALGINE_BINARY_DIR;BINARY_DIR" # list of names of mono-valued arguments
        "" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (NOT PARSED_ARG_TARGET)
        message(FATAL_ERROR "You must provide a target")
    endif()

    if (NOT PARSED_ARG_ALGINE_BINARY_DIR)
        message(DEBUG "ALGINE_BINARY_DIR is not specified, falling back to default")
        set(ALGINE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/algine")
    else()
        set(ALGINE_BINARY_DIR ${PARSED_ARG_ALGINE_BINARY_DIR})
    endif()

    if (NOT PARSED_ARG_BINARY_DIR)
        message(DEBUG "BINARY_DIR is not specified, falling back to default")
        set(BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
    else()
        set(BINARY_DIR ${PARSED_ARG_BINARY_DIR})
    endif()

    add_custom_command(TARGET ${PARSED_ARG_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -D CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}"
                -D ALGINE_BINARY_DIR="${ALGINE_BINARY_DIR}"
                -D BINARY_DIR="${BINARY_DIR}"
                -D ALGINE_DEPS_BUILD_DIR_NAME="${ALGINE_DEPS_DIR_NAME}-build"
                -P "${ALGINE_SOURCE_DIR}/cmake/win32/target_mklink.cmake"
            COMMENT "Creating symlinks..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endfunction()

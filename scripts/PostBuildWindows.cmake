# this file included in CMakeLists.txt

if (NOT WIN32)
    message(FATAL_ERROR "You can include PostBuildWindows.cmake only on Windows")
endif()

if (NOT DEFINED ALGINE_SOURCE_DIR)
    message(FATAL_ERROR "ALGINE_SOURCE_DIR is not defined")
endif()

# symlink dlls to exe directory
# it is necessary to run for your app on Windows
function(algine_target_mklink)
    cmake_parse_arguments(
        PARSED_ARG # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "TARGET;ALGINE_BINARY_DIR" # list of names of mono-valued arguments
        "" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (NOT PARSED_ARG_TARGET)
        message(FATAL_ERROR "You must provide a target")
    endif()

    if (NOT PARSED_ARG_ALGINE_BINARY_DIR)
        message(DEBUG "ALGINE_BINARY_DIR is not specified, CMAKE_CURRENT_BINARY_DIR will be used")
        set(ALGINE_BINARY_DIR ${PARSED_ARG_ALGINE_BINARY_DIR})
    endif()

    add_custom_command(TARGET ${PARSED_ARG_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -D CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
                -D ALGINE_BINARY_DIR=${PARSED_ARG_ALGINE_BINARY_DIR}
                -D ALGINE_DEPS_BUILD_DIR_NAME="${ALGINE_DEPS_DIR_NAME}-build"
                -P ${ALGINE_SOURCE_DIR}/scripts/target_mklink_win_script.cmake
            COMMENT "Creating symlinks..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endfunction()

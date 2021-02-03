# this file included in CMakeLists.txt

if (NOT WIN32)
    message(FATAL_ERROR "You can include PostBuildWindows.cmake only on Windows")
endif()

# symlink dlls to exe directory
# it is necessary to run for your app on Windows
function(algine_target_mklink_win target path_to_algine)
    add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -DCURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
                -DPATH_TO_ALGINE=${path_to_algine}
                -P ${path_to_algine}/cmake/target_mklink_win_script.cmake
            COMMENT "Creating symlinks..."
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endfunction()

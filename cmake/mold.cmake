function(try_use_mold)
    cmake_parse_arguments(
        PARSED_ARG # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "STATUS" # list of names of mono-valued arguments
        "" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (UNIX AND NOT ANDROID)
        execute_process(COMMAND bash -c "which mold > /dev/null" RESULT_VARIABLE moldRes)

        if (${moldRes} EQUAL 0)
            message(STATUS "mold was found and will be used for linking")

            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
            set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=mold")

            if (DEFINED PARSED_ARG_STATUS)
                set(${PARSED_ARG_STATUS} 1)
            endif()
        else()
            if (DEFINED PARSED_ARG_STATUS)
                set(${PARSED_ARG_STATUS} 0)
            endif()
        endif()
    endif()
endfunction()
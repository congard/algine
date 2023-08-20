function(build_solgen out_binary)
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/tools/solgen")
    set(solgen_dir "${CMAKE_CURRENT_LIST_DIR}/deps/solgen-src")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -B ${output_dir} -D CMAKE_C_COMPILER=${CMAKE_C_COMPILER} -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        WORKING_DIRECTORY ${solgen_dir})
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${output_dir} --target solgen -- -j 4
        WORKING_DIRECTORY ${solgen_dir})
    
    set(${out_binary} "${output_dir}/solgen" PARENT_SCOPE)
endfunction()

function(gen_type_list list_name save_to includes classes)
    foreach(include IN LISTS includes)
        set(includes_code "${includes_code}\n#include \"${CMAKE_CURRENT_LIST_DIR}/${include}\"")
    endforeach()

    string(REPLACE ";" ", " classes "${classes}")

    string(CONCAT code
        "#ifndef ALGINE_${list_name}_H\n"
        "#define ALGINE_${list_name}_H\n"
        "\n"
        "#include <algine/core/lua/Lua.h>\n"
        "${includes_code}\n"
        "\n"
        "namespace algine {\n"
        "using ${list_name} = Lua::TypeList<${classes}>;\n"
        "}\n"
        "\n"
        "#endif")

    if (EXISTS "${save_to}")
        file(READ "${save_to}" code_old)
    endif()
    
    if (NOT code_old STREQUAL code)
        file(WRITE "${save_to}" "${code}")  # rewrite only if it really necessary
        message(VERBOSE "Generated: ${save_to}")
    endif()
endfunction()

function(solgen output)
    message(STATUS "Solgen: starting")

    if (NOT ALGINE_SOLGEN_PATH)
        message(STATUS "ALGINE_SOLGEN_PATH is not specified, building solgen")
        build_solgen(ALGINE_SOLGEN_PATH)
    endif()

    message(STATUS "ALGINE_SOLGEN_PATH=${ALGINE_SOLGEN_PATH}")

    if (ANDROID)
        set(platform_str "android")
    elseif (UNIX)
        set(platform_str "unix")
    elseif (WIN32)
        set(platform_str "windows")
    else()
        message(FATAL_ERROR "Unsupported system")
    endif()

    function(run_lua)
        cmake_parse_arguments(
            PARSED_ARGS # prefix of output variables
            "" # list of names of the boolean arguments (only defined ones will be true)
            "OUTPUT_VAR;SCRIPT_FILE;WORKING_DIR" # list of names of mono-valued arguments
            "ARGS" # list of names of multi-valued arguments (output variables are lists)
            ${ARGN} # arguments of the function to parse, here we take the all original ones
        )

        if (NOT PARSED_ARGS_OUTPUT_VAR)
            message(FATAL_ERROR "You must provide an output variable")
        endif()

        if (NOT PARSED_ARGS_SCRIPT_FILE)
            message(FATAL_ERROR "You must provide a script file")
        endif()

        if (NOT PARSED_ARGS_WORKING_DIR)
            set(PARSED_ARGS_WORKING_DIR ".")
        endif()

        execute_process(COMMAND
            ${ALGINE_LUA_PATH} ${PARSED_ARGS_SCRIPT_FILE} ${PARSED_ARGS_ARGS}
            RESULT_VARIABLE run_lua_exit_code
            OUTPUT_VARIABLE ${PARSED_ARGS_OUTPUT_VAR}
            WORKING_DIRECTORY ${PARSED_ARGS_WORKING_DIR})

        if (run_lua_exit_code AND NOT run_lua_exit_code EQUAL 0)
            message(FATAL_ERROR "FindCompilerInclude.lua fatal error: ${${output_var}}")
        endif()

        set(${PARSED_ARGS_OUTPUT_VAR} ${${PARSED_ARGS_OUTPUT_VAR}} PARENT_SCOPE)
    endfunction()

    # get the list of enabled classes and its paths
    set(lists_file "${CMAKE_SOURCE_DIR}/SolgenLists.lua")

    if (NOT EXISTS "${lists_file}")
        message(VERBOSE "Solgen: file ${lists_file} does not exist, default lists will be used")
        set(lists_file "${CMAKE_CURRENT_LIST_DIR}/tools/solgen_lists/SolgenLists.default.lua")
    endif ()

    message(VERBOSE "Solgen lists: ${lists_file}")

    run_lua(SCRIPT_FILE
        "${CMAKE_CURRENT_LIST_DIR}/tools/solgen_lists/main.lua"
        WORKING_DIR "${CMAKE_CURRENT_LIST_DIR}/tools/solgen_lists"
        OUTPUT_VAR allowList
        ARGS "${lists_file}")

    if (allowList)
        list(GET allowList 0 enabledClasses)
        string(REPLACE "," ";" enabledClasses ${enabledClasses})

        list(GET allowList 1 enabledClassPaths)
        string(REPLACE "," ";" enabledClassPaths ${enabledClassPaths})

        message(VERBOSE "Solgen: enabled classes: ${enabledClasses}")
    else()
        message(VERBOSE "Solgen: allow list is empty, skipping generation")
        return()
    endif()

    set(solgenBindingsListFile "${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/AlgineBindings.h")

    gen_type_list(AlgineBindings "${solgenBindingsListFile}"
        "${enabledClassPaths}"
        "${enabledClasses}")

    string(REPLACE ";" "|" SOLGEN_CLASSES_REGEX "${enabledClasses}")

    # get compiler's include paths
    run_lua(SCRIPT_FILE
        "${CMAKE_CURRENT_LIST_DIR}/scripts/FindCompilerInclude.lua"
        OUTPUT_VAR compilerIncludes
        ARGS ${CMAKE_CXX_COMPILER} ${platform_str})

    string(REPLACE "\n" ";" compilerIncludes ${compilerIncludes})

    macro(append_clang_flags)
        list(APPEND ALGINE_SOLGEN_CLANG_ARGS ${ARGV})
    endmacro()

    # if Android, get API version as a number
    if (ANDROID)
        string(REPLACE "-" ";" androidApiList ${ANDROID_PLATFORM})
        list(LENGTH androidApiList androidApiListLen)

        if (${androidApiListLen} EQUAL 1)  # 32
            set(ANDROID_API_VER ${androidApiList})
        else()  # android-32
            list(GET androidApiList 1 ANDROID_API_VER)
        endif()

        set(SOLGEN_ANDROID_DEFS -D __ANDROID__ -D __ANDROID_API__=${ANDROID_API_VER})

        if (UNIX)
            append_clang_flags(-U __linux__ ${SOLGEN_ANDROID_DEFS})
        else()
            append_clang_flags(-U _WIN32 -U _WIN64 -U WIN32 -U WIN64 ${SOLGEN_ANDROID_DEFS})
        endif()
    elseif (ALGINE_QT_PLATFORM)
        append_clang_flags(-DALGINE_QT_PLATFORM -fPIC)
    endif()

    if (ALGINE_SECURE_OPERATIONS)
        append_clang_flags(-DALGINE_SECURE_OPERATIONS -DALGINE_SOP_LEVEL=${ALGINE_SOP_LEVEL})
    endif()

    message(VERBOSE "Solgen clang args: ${ALGINE_SOLGEN_CLANG_ARGS}")

    set(SOLGEN_COMMAND ${ALGINE_SOLGEN_PATH}
        ${ALGINE_SOLGEN_ARGS}
        --print-paths
        --input ${enabledClassPaths}
        --output-dir ${SOLGEN_OUTPUT_PATH}
        --output-namespace "algine_lua"
        --includes ${PRIVATE_INCLUDES} ${PUBLIC_INCLUDES} ${compilerIncludes} ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        --namespace-filter algine
        --type-filter "algine::(?:${SOLGEN_CLASSES_REGEX})(?:::[_a-zA-Z0-9]+)?$"
        --conf scripts/solgen.conf
        --clang-args -std=c++17 ${ALGINE_SOLGEN_CLANG_ARGS})

    message(DEBUG "Executing: ${SOLGEN_COMMAND}")

    execute_process(COMMAND ${SOLGEN_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE solgen_ret
        OUTPUT_VARIABLE ${output})

    string(REPLACE "\n" ";" ${output} ${${output}})
    set(${output}
            ${${output}}
            "${solgenBindingsListFile}"
            PARENT_SCOPE)

    if (solgen_ret AND NOT solgen_ret EQUAL 0)
        message(FATAL_ERROR "solgen fatal error: ${${output}}")
    endif()

    message(STATUS "Solgen: successfully generated")
endfunction()

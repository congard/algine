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

    function(set_if_empty var)
        if (NOT ${var})
            set(${var} ${ARGN} PARENT_SCOPE)
        endif()
    endfunction()

    set_if_empty(ALGINE_SOLGEN_INPUT_CORE
        include/common/algine/core/Object.h

        include/common/algine/core/lua/Scriptable.h

        include/common/algine/core/font/Font.h
        include/common/algine/core/font/FontLibrary.h
        include/common/algine/core/font/FontMetrics.h
        include/common/algine/core/font/FontRenderer.h

        include/common/algine/core/io/IOProvider.h

        include/common/algine/core/log/Log.h

        include/common/algine/core/shader/Shader.h
        include/common/algine/core/shader/ShaderBuilder.h
        include/common/algine/core/shader/ShaderDefinitionGenerator.h
        include/common/algine/core/shader/ShaderProgram.h
        include/common/algine/core/shader/ShaderProgramBuilder.h

        include/common/algine/core/Color.h

        include/common/algine/core/ImageBuilder.h

        include/common/algine/core/texture/Texture.h
        include/common/algine/core/texture/Texture2D.h
        include/common/algine/core/texture/Texture2DBuilder.h
        include/common/algine/core/texture/TextureCreateInfo.h
        include/common/algine/core/texture/TextureBuilder.h
        include/common/algine/core/texture/TextureCube.h
        include/common/algine/core/texture/TextureCubeBuilder.h
        include/common/algine/core/texture/TextureFileInfo.h

        include/common/algine/core/Framebuffer.h
        include/common/algine/core/FramebufferBuilder.h

        include/common/algine/core/Renderbuffer.h
        include/common/algine/core/RenderbufferBuilder.h

        include/common/algine/core/Builder.h
        include/common/algine/core/Object.h
        include/common/algine/core/DataType.h

        include/common/algine/core/OutputList.h

        include/common/algine/core/painter/Paint.h
        include/common/algine/core/painter/Painter.h
        include/common/algine/core/painter/RoundRect.h

        include/common/algine/core/widgets/Widget.h
        include/common/algine/core/widgets/Dimen.h
        include/common/algine/core/widgets/Unit.h)

    set_if_empty(ALGINE_SOLGEN_INPUT_STD
        include/common/algine/std/model/ShapeBuilder.h
        include/common/algine/std/model/Shape.h
        include/common/algine/std/model/ModelBuilder.h
        include/common/algine/std/model/Model.h
        include/common/algine/std/model/InputLayoutShapeLocations.h

        include/common/algine/std/rotator/Rotator.h
        include/common/algine/std/rotator/EulerRotator.h
        include/common/algine/std/rotator/FreeRotator.h

        include/common/algine/std/Rotatable.h
        include/common/algine/std/Scalable.h
        include/common/algine/std/Translatable.h

        include/common/algine/std/AMTLManager.h
        include/common/algine/std/AMTLMaterialManager.h)

    set_if_empty(ALGINE_SOLGEN_CORE_CLASSES
        Object
        Scriptable
        Font FontLibrary FontMetrics FontRenderer
        IOProvider
        Log
        Shader ShaderBuilder ShaderDefinitionGenerator ShaderProgram ShaderProgramBuilder
        Color
        ImageBuilder
        Texture Texture2D TextureCreateInfo TextureCube TextureFileInfo
        TextureBuilder Texture2DBuilder TextureCubeBuilder
        Framebuffer FramebufferBuilder
        Renderbuffer RenderbufferBuilder
        Builder Object
        DataType
        OutputList
        Paint Painter RoundRect
        Widget Dimen Unit)

    set_if_empty(ALGINE_SOLGEN_STD_CLASSES
        ShapeBuilder Shape
        ModelBuilder Model
        InputLayoutShapeLocations
        Rotator EulerRotator FreeRotator
        Rotatable Scalable Translatable
        AMTLManager AMTLMaterialManager)

    set(CORE_EXTRA_CLASSES
        RectI RectF PointI PointF)

    gen_type_list(AlgineCore "${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/AlgineCore.h"
        "${ALGINE_SOLGEN_INPUT_CORE}"
        "${ALGINE_SOLGEN_CORE_CLASSES};${CORE_EXTRA_CLASSES}")
    
    gen_type_list(AlgineStd "${CMAKE_CURRENT_BINARY_DIR}/src/common/core/lua/AlgineStd.h"
        "${ALGINE_SOLGEN_INPUT_STD}"
        "${ALGINE_SOLGEN_STD_CLASSES}")
    
    string(REPLACE ";" "|" SOLGEN_CORE_CLASSES_REGEX "${ALGINE_SOLGEN_CORE_CLASSES}")
    string(REPLACE ";" "|" SOLGEN_STD_CLASSES_REGEX "${ALGINE_SOLGEN_STD_CLASSES}")

    if (ANDROID)
        set(platform_str "android")
    elseif (UNIX)
        set(platform_str "unix")
    elseif (WIN32)
        set(platform_str "windows")
    else()
        message(FATAL_ERROR "Unsupported system")
    endif()

    # get compiler's include paths
    execute_process(COMMAND
        ${ALGINE_LUA_PATH} ${CMAKE_CURRENT_LIST_DIR}/scripts/FindCompilerInclude.lua ${CMAKE_CXX_COMPILER} ${platform_str}
        RESULT_VARIABLE FindCompilerInclude_ret
        OUTPUT_VARIABLE compilerIncludes)

    if (FindCompilerInclude_ret AND NOT FindCompilerInclude_ret EQUAL 0)
        message(FATAL_ERROR "FindCompilerInclude.lua fatal error: ${compilerIncludes}")
    endif()

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
        append_clang_flags(-DALGINE_QT_PLATFORM)
    endif()

    if (ALGINE_SECURE_OPERATIONS)
        append_clang_flags(-DALGINE_SECURE_OPERATIONS -DALGINE_SOP_LEVEL=${ALGINE_SOP_LEVEL})
    endif()

    message(DEBUG "solgen clang args: ${ALGINE_SOLGEN_CLANG_ARGS}")

    set(SOLGEN_COMMAND ${ALGINE_SOLGEN_PATH}
        ${ALGINE_SOLGEN_ARGS}
        --print-paths
        --input ${ALGINE_SOLGEN_INPUT_CORE} ${ALGINE_SOLGEN_INPUT_STD}
        --output-dir ${SOLGEN_OUTPUT_PATH}
        --output-namespace "algine_lua"
        --includes ${PRIVATE_INCLUDES} ${PUBLIC_INCLUDES} ${compilerIncludes} ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        --namespace-filter algine
        --type-filter "algine::(?:${SOLGEN_CORE_CLASSES_REGEX}|${SOLGEN_STD_CLASSES_REGEX})(?:::[_a-zA-Z0-9]+)?$"
        --conf scripts/solgen.conf
        --clang-args -std=c++17 ${ALGINE_SOLGEN_CLANG_ARGS})

    message(DEBUG "Executing: ${SOLGEN_COMMAND}")

    execute_process(COMMAND ${SOLGEN_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE solgen_ret
        OUTPUT_VARIABLE ${output})

    string(REPLACE "\n" ";" ${output} ${${output}})
    set(${output} ${${output}} PARENT_SCOPE)

    if (solgen_ret AND NOT solgen_ret EQUAL 0)
        message(FATAL_ERROR "solgen fatal error: ${${output}}")
    endif()

    message(STATUS "Solgen: successfully generated")
endfunction()

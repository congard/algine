function(build_solgen out_binary)
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/tools/solgen")
    set(solgen_dir "${CMAKE_CURRENT_LIST_DIR}/libs/solgen")

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
        message(STATUS "ALGINE_SOLGEN_PATH=${ALGINE_SOLGEN_PATH}")
    endif()

    function(set_if_empty var)
        if (NOT ${var})
            set(${var} ${ARGN} PARENT_SCOPE)
        endif()
    endfunction()

    set_if_empty(ALGINE_SOLGEN_INPUT_CORE
        include/common/algine/core/lua/Scriptable.h

        include/common/algine/core/font/Font.h
        include/common/algine/core/font/FontLibrary.h
        include/common/algine/core/font/FontMetrics.h
        include/common/algine/core/font/FontRenderer.h

        include/common/algine/core/io/IOProvider.h

        include/common/algine/core/log/Log.h

        include/common/algine/core/shader/Shader.h
        include/common/algine/core/shader/ShaderCreator.h
        include/common/algine/core/shader/ShaderDefinitionGenerator.h
        include/common/algine/core/shader/ShaderProgram.h
        include/common/algine/core/shader/ShaderProgramCreator.h

        include/common/algine/core/Color.h

        include/common/algine/core/ImageCreator.h

        include/common/algine/core/texture/Texture.h
        include/common/algine/core/texture/Texture2D.h
        include/common/algine/core/texture/Texture2DCreator.h
        include/common/algine/core/texture/TextureCreateInfo.h
        include/common/algine/core/texture/TextureCreator.h
        include/common/algine/core/texture/TextureCube.h
        include/common/algine/core/texture/TextureCubeCreator.h
        include/common/algine/core/texture/TextureFileInfo.h

        include/common/algine/core/Framebuffer.h
        include/common/algine/core/FramebufferCreator.h

        include/common/algine/core/Renderbuffer.h
        include/common/algine/core/RenderbufferCreator.h

        include/common/algine/core/Creator.h
        include/common/algine/core/Object.h
        include/common/algine/core/DataType.h

        include/common/algine/core/OutputList.h

        include/common/algine/core/painter/Paint.h
        include/common/algine/core/painter/Painter.h
        include/common/algine/core/painter/RoundRect.h

        include/common/algine/core/widgets/Widget.h)

    set_if_empty(ALGINE_SOLGEN_INPUT_STD
        include/common/algine/std/model/ShapeCreator.h
        include/common/algine/std/model/Shape.h
        include/common/algine/std/model/ModelCreator.h
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
        Scriptable
        Font FontLibrary FontMetrics FontRenderer
        IOProvider
        Log
        Shader ShaderCreator ShaderDefinitionGenerator ShaderProgram ShaderProgramCreator
        Color
        ImageCreator
        Texture Texture2D TextureCreateInfo TextureCube TextureFileInfo
        TextureCreator Texture2DCreator TextureCubeCreator
        Framebuffer FramebufferCreator
        Renderbuffer RenderbufferCreator
        Creator Object
        DataType
        OutputList
        Paint Painter RoundRect
        Widget)

    set_if_empty(ALGINE_SOLGEN_STD_CLASSES
        ShapeCreator Shape
        ModelCreator Model
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
            set(ALGINE_SOLGEN_CLANG_ARGS ${ALGINE_SOLGEN_CLANG_ARGS} -U __linux__ ${SOLGEN_ANDROID_DEFS})
        else()
            set(ALGINE_SOLGEN_CLANG_ARGS ${ALGINE_SOLGEN_CLANG_ARGS} -U _WIN32 -U _WIN64 -U WIN32 -U WIN64 ${SOLGEN_ANDROID_DEFS})
        endif()
    endif()

    set(SOLGEN_COMMAND ${ALGINE_SOLGEN_PATH}
        ${ALGINE_SOLGEN_ARGS}
        --print-paths
        --input ${ALGINE_SOLGEN_INPUT_CORE} ${ALGINE_SOLGEN_INPUT_STD}
        --output-dir ${SOLGEN_OUTPUT_PATH}
        --output-namespace "algine_lua"
        --includes ${PRIVATE_INCLUDES} ${PUBLIC_INCLUDES} ${compilerIncludes}
        --namespace-filter algine
        --type-filter "algine::(?:${SOLGEN_CORE_CLASSES_REGEX}|${SOLGEN_STD_CLASSES_REGEX})(?:::[_a-zA-Z0-9]+)?$"
        --conf scripts/solgen.conf
        --clang-args -std=c++17 ${ALGINE_SOLGEN_CLANG_ARGS})

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
# Reads files to variables.
# BASEDIR: The directory files located in
# PAIRS: [file0 out-var0 file1 out-var1 ...]
macro(_glsl_read)
    cmake_parse_arguments(
        ARG # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "BASEDIR" # list of names of mono-valued arguments
        "PAIRS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (NOT ARG_BASEDIR)
        set(ARG_BASEDIR "")
    else()
        set(ARG_BASEDIR "${ARG_BASEDIR}/")
    endif()

    list(LENGTH ARG_PAIRS PAIRS_LEN)

    MATH(EXPR RANGE_END "${PAIRS_LEN}-1")

    foreach(i RANGE 0 ${RANGE_END} 2)
        list(GET ARG_PAIRS ${i} file)

        MATH(EXPR i "${i}+1")
        list(GET ARG_PAIRS ${i} varname)

        file(READ "${ARG_BASEDIR}${file}" "${varname}")
    endforeach()
endmacro()

# built-in glsl modules
function(glsl_pack_modules)
    set(MDIR "src/common/core/glsl-modules")

    _glsl_read(BASEDIR "${MDIR}" PAIRS
        "BoneSystem.glsl" GLSLModules_BoneSystem
        "NormalMapping.vs.glsl" GLSLModules_NormalMapping_vs
        "NormalMapping.fs.glsl" GLSLModules_NormalMapping_fs
        "SSR.glsl" GLSLModules_SSR

        "fresnel.glsl" GLSLModules_fresnel

        "ToneMapping/all.glsl" GLSLModules_ToneMapping_all
        "ToneMapping/aces.glsl" GLSLModules_ToneMapping_aces
        "ToneMapping/exposure.glsl" GLSLModules_ToneMapping_exposure
        "ToneMapping/filmic.glsl" GLSLModules_ToneMapping_filmic
        "ToneMapping/lottes.glsl" GLSLModules_ToneMapping_lottes
        "ToneMapping/reinhard.glsl" GLSLModules_ToneMapping_reinhard
        "ToneMapping/reinhard2.glsl" GLSLModules_ToneMapping_reinhard2
        "ToneMapping/uchimura.glsl" GLSLModules_ToneMapping_uchimura
        "ToneMapping/uncharted2.glsl" GLSLModules_ToneMapping_uncharted2
        "ToneMapping/unreal.glsl" GLSLModules_ToneMapping_unreal

        "Blending/all.glsl" GLSLModules_Blending_all
        "Blending/add.glsl" GLSLModules_Blending_add
        "Blending/average.glsl" GLSLModules_Blending_average
        "Blending/color-burn.glsl" GLSLModules_Blending_color_burn
        "Blending/color-dodge.glsl" GLSLModules_Blending_color_dodge
        "Blending/darken.glsl" GLSLModules_Blending_darken
        "Blending/difference.glsl" GLSLModules_Blending_difference
        "Blending/exclusion.glsl" GLSLModules_Blending_exclusion
        "Blending/glow.glsl" GLSLModules_Blending_glow
        "Blending/hard-light.glsl" GLSLModules_Blending_hard_light
        "Blending/hard-mix.glsl" GLSLModules_Blending_hard_mix
        "Blending/lighten.glsl" GLSLModules_Blending_lighten
        "Blending/linear-burn.glsl" GLSLModules_Blending_linear_burn
        "Blending/linear-dodge.glsl" GLSLModules_Blending_linear_dodge
        "Blending/linear-light.glsl" GLSLModules_Blending_linear_light
        "Blending/multiply.glsl" GLSLModules_Blending_multiply
        "Blending/negation.glsl" GLSLModules_Blending_negation
        "Blending/normal.glsl" GLSLModules_Blending_normal
        "Blending/overlay.glsl" GLSLModules_Blending_overlay
        "Blending/phoenix.glsl" GLSLModules_Blending_phoenix
        "Blending/pin-light.glsl" GLSLModules_Blending_pin_light
        "Blending/reflect.glsl" GLSLModules_Blending_reflect
        "Blending/screen.glsl" GLSLModules_Blending_screen
        "Blending/soft-light.glsl" GLSLModules_Blending_soft_light
        "Blending/subtract.glsl" GLSLModules_Blending_subtract
        "Blending/vivid-light.glsl" GLSLModules_Blending_vivid_light

        "Shading/all.glsl" GLSLModules_Shading_all
        "Shading/attenuation.glsl" GLSLModules_Shading_attenuation
        "Shading/diffuseLambert.glsl" GLSLModules_Shading_diffuseLambert
        "Shading/diffuseOrenNayar.glsl" GLSLModules_Shading_diffuseOrenNayar
        "Shading/specularAnisotropic.glsl" GLSLModules_Shading_specularAnisotropic
        "Shading/specularBlinnPhong.glsl" GLSLModules_Shading_specularBlinnPhong
        "Shading/specularCookTorrance.glsl" GLSLModules_Shading_specularCookTorrance
        "Shading/specularPhong.glsl" GLSLModules_Shading_specularPhong
        "Shading/dirLightShadow.glsl" GLSLModules_Shading_dirLightShadow
        "Shading/pointLightShadow.glsl" GLSLModules_Shading_pointLightShadow

        "DOF/all.glsl" GLSLModules_DOF_all
        "DOF/cinematicCoC.glsl" GLSLModules_DOF_cinematicCoC
        "DOF/linearCoC.glsl" GLSLModules_DOF_linearCoC

        "Luminance/all.glsl" GLSLModules_Luminance_all
        "Luminance/luminance.glsl" GLSLModules_Luminance_luminance
        "Luminance/luminanceHSP.glsl" GLSLModules_Luminance_luminanceHSP
        "Luminance/luminanceW3.glsl" GLSLModules_Luminance_luminanceW3)

    configure_file("${MDIR}/GLSLModules.h.in" "${MDIR}/GLSLModules.h")
endfunction()

# built-in glsl shaders
function(glsl_pack_shaders)
    set(SDIR "src/common/core/glsl-shaders")

    _glsl_read(BASEDIR "${SDIR}" PAIRS
        "Blur.fs.glsl" GLSLShaders_Blur_fs
        "Quad.vs.glsl" GLSLShaders_Quad_vs
        "Shadow.fs.glsl" GLSLShaders_Shadow_fs
        "Shadow.gs.glsl" GLSLShaders_Shadow_gs
        "Shadow.vs.glsl" GLSLShaders_Shadow_vs)

    configure_file("${SDIR}/GLSLShaders.h.in" "${SDIR}/GLSLShaders.h")
endfunction()

# painter-related shaders
function(glsl_pack_painter_shaders)
    set(PDIR "src/common/core/painter")

    _glsl_read(BASEDIR "${PDIR}" PAIRS
        "painter.vs.glsl" Painter_painter_vs
        "fill.fs.glsl" Painter_fill_fs
        "circle-fill.fs.glsl" Painter_circle_fill_fs
        "roundRect-fill.fs.glsl" Painter_round_rect_fill_fs
        "text-fill.fs.glsl" Painter_text_fill_fs)

    configure_file("${PDIR}/PainterShaders.h.in" "${PDIR}/PainterShaders.h")
endfunction()

function(glsl_pack_all)
    glsl_pack_modules()
    glsl_pack_shaders()
    glsl_pack_painter_shaders()
endfunction()
#ifndef ALGINE_STRUCTS
#define ALGINE_STRUCTS

#include <iostream>
#include <GL/glew.h>
#include "constants.h"

namespace algine {
struct AlginePrograms {
    // CS - color shader
    GLuint
        // vertex shader
        CS_MAT_MODEL,
        CS_MAT_VIEW,
        CS_MAT_PVM,
        CS_MAT_VM,
        CS_SWITCH_NORMAL_MAPPING,
        CS_IN_POSITION,
        CS_IN_NORMAL,
        CS_IN_TANGENT,
        CS_IN_BITANGENT,
        CS_IN_TEXCOORD,

        // fragment shader
        CS_VIEW_POSITION,
        CS_LAMPS_COUNT,
        CS_BRIGHTNESS_THRESHOLD,
        CS_SHADOW_FAR_PLANE,
        CS_SHADOW_DISKRADIUS_K,
        CS_SHADOW_DISKRADIUS_MIN,
        CS_SHADOW_BIAS,
        CS_FOCAL_DEPTH,
        CS_FOCAL_RANGE,
        //*CS_SHADOW_MAPS, // The loading of these locations is performed by the class TextureArray (utils/texture_utils.cpp)
        CS_SAMPLER_MAPPING_AMBIENT,
        CS_SAMPLER_MAPPING_DIFFUSE,
        CS_SAMPLER_MAPPING_SPECULAR,
        CS_SAMPLER_MAPPING_NORMAL,
        CS_SAMPLER_MAPPING_REFLECTIONSTRENGTH,
        CS_SAMPLER_MAPPING_JITTER,
        CS_COLOR_MAPPING_AMBIENT,
        CS_COLOR_MAPPING_DIFFUSE,
        CS_COLOR_MAPPING_SPECULAR,
        CS_SWITCH_TEXTURE_MAPPING;
    // *CS_LAMPS; // The loading of these locations is performed by the class Lamp (lamp.cpp)

    // SS - shadow shader
    GLuint
        // vertex shader
        SS_MAT_MODEL,
        SS_IN_POSITION,

        // geometry shader
        // SS_MAT_SHADOW[6], // The loading of these locations is performed by the class Lamp (lamp.cpp)

        // fragment shader
        SS_LAMP_POSITION,
        SS_FAR_PLANE;

    // BLUS_HORIZONTAL - blur horizontal shader
    GLuint
        // vertex shader
        BLUS_H_IN_POSITION,
        BLUS_H_IN_TEXCOORD,

        // fragment shader
        BLUS_H_SAMPLER_BLOOM,
        BLUS_H_SAMPLER_SCENE,
        BLUS_H_SAMPLER_DOF_BUFFER,
        BLUS_H_SIGMA_MIN,
        BLUS_H_SIGMA_MAX,
        *BLUS_H_KERNEL_BLOOM;

    // BLUS_VERTICAL - blur horizontal shader
    GLuint
        // vertex shader
        BLUS_V_IN_POSITION,
        BLUS_V_IN_TEXCOORD,

        // fragment shader
        BLUS_V_SAMPLER_BLOOM,
        BLUS_V_SAMPLER_SCENE,
        BLUS_V_SAMPLER_DOF_BUFFER,
        BLUS_V_SIGMA_MIN,
        BLUS_V_SIGMA_MAX,
        *BLUS_V_KERNEL_BLOOM;

    // BLES - blend shader
    GLuint
        // vertex shader
        BLES_IN_POSITION,
        BLES_IN_TEXCOORD,

        // fragment shader
        BLES_SAMPLER_BLOOM_SCENE,
        BLES_SAMPLER_DOF_SCENE,
        BLES_EXPOSURE,
        BLES_GAMMA;

    // SSS - screen space shader
    GLuint
        // vertex shader
        SSS_IN_POSITION,
        SSS_IN_TEXCOORD,

        // fragment shader
        SSS_SAMPLER_MAP_COLOR,
        SSS_SAMPLER_MAP_NORMAL,
        SSS_SAMPLER_MAP_SSRVALUES,
        SSS_SAMPLER_MAP_POSITION,
        SSS_MAT_PROJECTION,
        SSS_MAT_VIEW,

        SSS_SSR_SKYCOLOR,
        SSS_SSR_BINARYSEARCHCOUNT,
        SSS_SSR_RAYMARCHCOUNT,
        SSS_SSR_STEP,
        SSS_SSR_LLIMITER,
        SSS_SSR_MINRAYSTEP,

        SSS_BL_VECTHRESHOLD,
        SSS_BL_BRIGHTNESSTHRESHOLD;

    GLuint
        PROGRAM_ID_CS,
        PROGRAM_ID_SS,
        PROGRAM_ID_SSS,
        PROGRAM_ID_BLUS_HORIZONTAL,
        PROGRAM_ID_BLUS_VERTICAL,
        PROGRAM_ID_BLES;
};

struct AlgineParams {
    GLubyte
        NORMAL_MAPPING_MODE = ALGINE_NORMAL_MAPPING_MODE_ENABLED,
        SHADOW_MAPPING_MODE = ALGINE_SHADOW_MAPPING_MODE_ENABLED,
        BLOOM_MODE = ALGINE_BLOOM_MODE_ENABLED,
        DOF_MODE = ALGINE_DOF_MODE_ENABLED,
        TEXTURE_MAPPING_MODE = ALGINE_TEXTURE_MAPPING_MODE_ENABLED,
        LIGHTING_MODE = ALGINE_LIGHTING_MODE_ENABLED,
        ATTENUATION_MODE = ALGINE_ATTENUATION_MODE_ENABLED,
        SSR_MODE = ALGINE_SSR_MODE_ENABLED;

    GLuint
        MAX_LAMPS_COUNT = 8,
        KERNEL_BLOOM_SIZE = 8,
        KERNEL_DOF_SIZE = 8,
        SCR_W = 1366,
        SCR_H = 768,
        BLUR_AMOUNT = 4;

    GLfloat
        GAMMA = 1,
        EXPOSURE = 3,
        KERNEL_BLOOM_SIGMA = 3;
};

struct ShadersData {
    std::string
        vertex,
        fragment,
        geometry = "";
};

#define ShadersPaths ShadersData

} // namespace algine

#endif /* ALGINE_STRUCTS */
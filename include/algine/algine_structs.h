#ifndef ALGINE_STRUCTS
#define ALGINE_STRUCTS

#include <algine/constants.h>
#include <algine/types.h>

// TODO: remove all structs *Params!
//  ...and algine_structs.h

namespace algine {
struct ColorShaderParams {
    uint
        maxPointLightsCount = 8,
        maxDirLightsCount = 8,
        maxBoneAttribsPerVertex = 1, // 1 bone attrib = 4 bones per vertex
        maxBones = 64; // max bones in mesh
};

struct ShadowShaderParams {
    uint
        maxBoneAttribsPerVertex = 1, // 1 bone attrib = 4 bones per vertex
        maxBones = 64; // max bones in mesh
};

struct CubemapShaderParams {
    uint
        cubemapColorOutColorComponent = ALGINE_OUT_CS_COLOR_COMPONENT_COLOR,
        positionOutColorComponent = ALGINE_OUT_CS_COLOR_COMPONENT_POSITION,
        vecout = ALGINE_VEC3,
        cubemapColorOutput = ALGINE_ENABLED,
        positionOutput = ALGINE_DISABLED;
};

struct BlurShaderParams {
    uint
        blurKernelRadius = 4,
        vecout = ALGINE_VEC3,
        texComponent = ALGINE_SHADER_TEX_COMPONENT_RGB;
};

struct DOFBlurShaderParams {
    uint
        blurKernelRadius,
        type,
        bleedingEliminationDeltaZ = ALGINE_DISABLED,
        bleedingEliminationDeltaCoC = ALGINE_DISABLED,
        bleedingEliminationFocusCoC = ALGINE_DISABLED;
};

struct AlgineParams {
    uint
        normalMappingMode = ALGINE_NORMAL_MAPPING_MODE_ENABLED,
        shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_DISABLED,
        shadowMappingType = ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING,
        bloomMode = ALGINE_BLOOM_MODE_ENABLED,
        bloomType = ALGINE_BLOOM_TYPE_ADD,
        textureMappingMode = ALGINE_TEXTURE_MAPPING_MODE_ENABLED,
        lightingMode = ALGINE_LIGHTING_MODE_ENABLED,
        attenuationMode = ALGINE_ATTENUATION_MODE_ENABLED,
        ssrMode = ALGINE_SSR_MODE_ENABLED,
        boneSystemMode = ALGINE_BONE_SYSTEM_DISABLED;
};
} // namespace algine

#endif /* ALGINE_STRUCTS */

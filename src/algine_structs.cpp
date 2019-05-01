#ifndef ALGINE_STRUCTS
#define ALGINE_STRUCTS

#include <string>
#include <GL/glew.h>
#include <algine/constants.h>
#include <algine/types.h>

namespace algine {
// color shader ids
struct CShader {
    GLint
        programId,

        // vertex shader
        matModel,
        matView,
        matPVM, // projection * view * model
        matVM,
        bones, // zero element id
        normalMappingSwitcher,
        boneAttribsPerVertex, // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)
        inPosition,
        inNormal,
        inTangent,
        inBitangent,
        inTexCoord,
        inBoneIds, // zero element id
        inBoneWeights, // zero element id

        // fragment shader
        viewPos,
        lampsCount,
        shadowDiskRadiusK,
        shadowDiskRadiusMin,
        shadowBias,
        shadowOpacity,
        focalDepth,
        focalRange,
        materialAmbientTex,
        materialDiffuseTex,
        materialSpecularTex,
        materialNormalTex,
        materialReflectionStrengthTex,
        materialJitterTex,
        materialAmbientColor,
        materialDiffuseColor,
        materialSpecularColor,
        materialAmbientStrength,
        materialDiffuseStrength,
        materialSpecularStrength,
        materialShininess,
        textureMappingSwitcher,
        cinematicDOFPlaneInFocus,
        cinematicDOFAperture,
        cinematicDOFImageDistance;
};

// shadow shader ids
struct SShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inBoneIds, // zero element id
        inBoneWeights, // zero element id
        matModel,
        bones, // zero element id
        boneAttribsPerVertex, // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)

        // fragment shader
        lampPos,
        far;
};

// blur shader ids
struct BLUShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerBloom,
        samplerScene,
        samplerDofBuffer,
        sigmaMin,
        sigmaMax,
        *bloomKernel;
};

// blend shader ids
struct BLEShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerBloomScene,
        samplerDofScene,
        exposure,
        gamma;
};

// screen space shader ids
struct SSShader {
    // SSS - screen space shader
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerColorMap,
        samplerNormalMap,
        samplerSSRValuesMap,
        samplerPositionMap,
        matProjection,
        matView,

        ssrSkyColor,
        ssrBinarySearchCount,
        ssrRayMarchCount,
        ssrStep,
        ssrLLimiter,
        ssrMinRayStep,

        blVecThreshold,
        blBrightnessThreshold;
};

struct AlgineParams {
    uint8
        normalMappingMode = ALGINE_NORMAL_MAPPING_MODE_ENABLED,
        shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_SIMPLE,
        bloomMode = ALGINE_BLOOM_MODE_ENABLED,
        dofMode = ALGINE_DOF_MODE_ENABLED,
        textureMappingMode = ALGINE_TEXTURE_MAPPING_MODE_ENABLED,
        lightingMode = ALGINE_LIGHTING_MODE_ENABLED,
        attenuationMode = ALGINE_ATTENUATION_MODE_ENABLED,
        ssrMode = ALGINE_SSR_MODE_ENABLED,
        boneSystemMode = ALGINE_BONE_SYSTEM_DISABLED;

    uint
        maxLampsCount = 8,
        maxBoneAttribsPerVertex = 1, // 1 bone attrib = 4 bones per vertex
        maxBones = 64, // max bones in mesh
        bloomKernelSize = 8,
        dofKernelSize = 8,
        scrW = 1366,
        scrH = 768,
        blurAmount = 4;

    float
        gamma = 1,
        exposure = 3,
        bloomKernelSigma = 3;
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

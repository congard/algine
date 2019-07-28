#ifndef ALGINE_STRUCTS
#define ALGINE_STRUCTS

#include <string>
#include <vector>
#include <GL/glew.h>
#include <algine/constants.h>
#include <algine/types.h>

namespace algine {

struct LightIds {
    int
        kc,
        kl,
        kq,
        pos,
        color,
        shadowMap;
};

struct DirLightIds : public LightIds {
    int lightMatrix, minBias, maxBias;
};

struct PointLightIds : public LightIds {
    int far, bias;
};

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
        pointLightsCount,
        dirLightsCount,
        shadowDiskRadiusK,
        shadowDiskRadiusMin,
        shadowOpacity,
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
        textureMappingSwitcher;

    std::vector<PointLightIds> pointLights;
    std::vector<DirLightIds> dirLights;
};

// shadow shader ids
struct SShader {
    GLint
        programId,

        // geometry shader
        shadowMatrices, //  zero element id

        // vertex shader
        inPosition,
        inBoneIds, // zero element id
        inBoneWeights, // zero element id
        matLightSpace,
        matModel,
        bones, // zero element id
        boneAttribsPerVertex, // bonesPerVertex / 4 + (bonesPerVertex % 4 == 0 ? 0 : 1)

        // fragment shader
        lampPos,
        far;
};

// dof blur shader ids
struct DOFBlurShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerImage,
        samplerPositionMap,
        samplerCoCMap,
        sigmaMin,
        sigmaMax,
        focalDepth,
        focalRange,
        cinematicDOFPlaneInFocus,
        cinematicDOFAperture,
        cinematicDOFImageDistance,
        bleedingEliminationMinDeltaZ,
        bleedingEliminationMinDeltaCoC,
        bleedingEliminationMaxFocusCoC;
};

struct DOFCoCShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerPositionMap,
        sigmaMin,
        sigmaMax,
        focalDepth,
        focalRange,
        cinematicDOFPlaneInFocus,
        cinematicDOFAperture,
        cinematicDOFImageDistance;
};

// blend shader ids
struct BlendShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        samplerBloom,
        samplerImage,
        exposure,
        gamma;
};

// screen space reflections shader ids
struct SSRShader {
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

        skyColor,
        binarySearchCount,
        rayMarchCount,
        step,
        LLimiter,
        minRayStep;
};

struct BloomSearchShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        threshold,
        brightnessThreshold,
        image;
};

struct BlurShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        inTexCoord,

        // fragment shader
        image,
        kernel; // zero element id
};

struct CubemapShader {
    GLint
        programId,

        // vertex shader
        inPosition,
        matTransform,
        
        // fragment shader
        cubemap,
        color,
        positionScaling,
        matView;
};

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

struct ShadersData {
    std::string
        vertex,
        fragment,
        geometry = "";
};

#define ShadersPaths ShadersData

} // namespace algine

#endif /* ALGINE_STRUCTS */

#ifndef ALGINE_STRUCTS
#define ALGINE_STRUCTS

#include <iostream>
#include <GL/glew.h>
#include "constants.h"

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
        normalMappingSwitcher,
        inPosition,
        inNormal,
        inTangent,
        inBitangent,
        inTexCoord,

        // fragment shader
        viewPos,
        lampsCount,
        shadowDiskRadiusK,
        shadowDiskRadiusMin,
        shadowBias,
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
        matModel,
        inPosition,

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
    GLubyte
        normalMappingMode = ALGINE_NORMAL_MAPPING_MODE_ENABLED,
        shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_SIMPLE,
        bloomMode = ALGINE_BLOOM_MODE_ENABLED,
        dofMode = ALGINE_DOF_MODE_ENABLED,
        textureMappingMode = ALGINE_TEXTURE_MAPPING_MODE_ENABLED,
        lightingMode = ALGINE_LIGHTING_MODE_ENABLED,
        attenuationMode = ALGINE_ATTENUATION_MODE_ENABLED,
        ssrMode = ALGINE_SSR_MODE_ENABLED;

    GLuint
        maxLampsCount = 8,
        bloomKernelSize = 8,
        dofKernelSize = 8,
        scrW = 1366,
        scrH = 768,
        blurAmount = 4;

    GLfloat
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

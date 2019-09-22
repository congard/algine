#include <algine/shader_compiler.h>
#include <algine/shader_program.h>
#include <algine/constants.h>
#include <algine/io.h>
#include <algine/core_utils.h>

namespace algine {
namespace scompiler { // shader compiler
void saveShaders(const ShadersData &shader, const ShadersPaths &path) {
    FILE *file = nullptr;
    io::save(file, shader.vertex, path.vertex);
    io::save(file, shader.fragment, path.fragment);
    if (!shader.geometry.empty())
        io::save(file, shader.geometry, path.geometry);
}

void loadColorShaderLocations(ShaderProgram &shader, const uint maxDirLightsCount, const uint maxPointLightsCount) {
    using namespace AlgineNames::ColorShader;

    std::vector<std::string> attribs {
        InPos, InNormal, InTexCoord, InTangent, InBitangent, InBoneIds, InBoneWeights
    };

    std::vector<std::string> uniforms {
        // vertex shader
        ModelMatrix, ViewMatrix, MVPMatrix, MVMatrix, Material::IsNormalMappingEnabled, Bones, BoneAttribsPerVertex,

        // fragment shader
        CameraPos, PointLightsCount, DirLightsCount, ShadowDiskRadiusK, ShadowDiskRadiusMin, ShadowOpacity, Material::IsTextureMappingEnabled,
        // material
        Material::AmbientTex, Material::DiffuseTex, Material::SpecularTex, Material::NormalTex, Material::ReflectionStrengthTex, Material::JitterTex,
        Material::AmbientColor, Material::DiffuseColor, Material::SpecularColor,
        Material::AmbientStrength, Material::DiffuseStrength, Material::SpecularStrength, Material::Shininess
    };

    shader.loadAttribLocations(attribs);
    shader.loadUniformLocations(uniforms);

    #define loadLightProp \
        shader.loadUniformLocation(start + Light::Kc); \
        shader.loadUniformLocation(start + Light::Kl); \
        shader.loadUniformLocation(start + Light::Kq); \
        shader.loadUniformLocation(start + Light::Pos); \
        shader.loadUniformLocation(start + Light::Color);

    // uniform name start
    std::string start;

    // point lights
    shader.loadUniformLocation(PointLightShadowMaps);
    for (uint i = 0; i < maxPointLightsCount; i++) {
        start = PointLights + std::string("[") + std::to_string(i) + "].";
        loadLightProp
        shader.loadUniformLocation(start + Light::FarPlane);
        shader.loadUniformLocation(start + Light::Bias);
    }

    // directional lights
    shader.loadUniformLocation(DirLightShadowMaps);
    for (uint i = 0; i < maxDirLightsCount; i++) {
        start = DirLights + std::string("[") + std::to_string(i) + "].";
        loadLightProp
        shader.loadUniformLocation(start + Light::LightMatrix);
        shader.loadUniformLocation(start + Light::MinBias);
        shader.loadUniformLocation(start + Light::MaxBias);
    }

    #undef loadLightProp
}

void loadShadowShaderLocations(ShaderProgram &shader) {
    using namespace AlgineNames::ShadowShader;

    std::vector<std::string> uniforms {
        // geometry shader
        PointLight::ShadowMatrices,

        // vertex shader
        TransformationMatrix, Bones, BoneAttribsPerVertex,

        // fragment shader
        PointLight::Pos, PointLight::FarPlane
    };

    shader.loadAttribLocation(InPos);
    shader.loadAttribLocation(InBoneIds);
    shader.loadAttribLocation(InBoneWeights);
    shader.loadUniformLocations(uniforms);
}

#define _loadDOFAttribLocations(shader) \
    shader.loadAttribLocation(AlgineNames::QuadShader::InPos); \
    shader.loadAttribLocation(AlgineNames::QuadShader::InTexCoord);

#define _getDOFUniformLocations \
    /* fragment shader */ \
    PositionMap, \
    /* linear DOF parameters */ \
    FocalDepth, FocalRange, MinSigma, MaxSigma, \
    /* cinematic DOF parameters */ \
    PlaneInFocus, Aperture, ImageDistance

void loadDOFBlurShaderLocations(ShaderProgram &shader) {
    using namespace AlgineNames::DOFShader;

    _loadDOFAttribLocations(shader)

    std::vector<std::string> uniforms {
        _getDOFUniformLocations,
        BaseImage, CoCMap,
        BleedingMinDeltaZ, BleedingMinDeltaCoC, BleedingMaxFocusCoC
    };
    shader.loadUniformLocations(uniforms);
}

void loadDOFCoCShaderLocations(ShaderProgram &shader) {
    using namespace AlgineNames::DOFShader;

    _loadDOFAttribLocations(shader)

    std::vector<std::string> uniforms {
        _getDOFUniformLocations
    };
    shader.loadUniformLocations(uniforms);
}

void loadBlendBloomShaderLocations(ShaderProgram &shader) {
    shader.loadAttribLocation(AlgineNames::QuadShader::InPos);
    shader.loadAttribLocation(AlgineNames::QuadShader::InTexCoord);

    // fragment shader
    shader.loadUniformLocation(AlgineNames::BlendBloomShader::BloomImage);
    shader.loadUniformLocation(AlgineNames::BlendBloomShader::BaseImage);
    shader.loadUniformLocation(AlgineNames::BlendBloomShader::Exposure);
    shader.loadUniformLocation(AlgineNames::BlendBloomShader::Gamma);
}

void loadSSRShaderLocations(ShaderProgram &shader) {
    using namespace AlgineNames::SSRShader;

    shader.loadAttribLocation(AlgineNames::QuadShader::InPos);
    shader.loadAttribLocation(AlgineNames::QuadShader::InTexCoord);

    // fragment shader
    std::vector<std::string> uniforms {
        BaseImage, NormalMap, SSRValuesMap, PositionMap,
        ProjectionMatrix, ViewMatrix, SkyColor, BinarySearchCount, RayMarchCount, RayStep, MinRayStep, LLimiter
    };
    shader.loadUniformLocations(uniforms);
}

void loadBloomSearchShaderLocations(ShaderProgram &shader) {
    shader.loadAttribLocation(AlgineNames::QuadShader::InPos);
    shader.loadAttribLocation(AlgineNames::QuadShader::InTexCoord);

    // fragment shader
    shader.loadUniformLocation(AlgineNames::BloomSearchShader::Threshold);
    shader.loadUniformLocation(AlgineNames::BloomSearchShader::BrightnessThreshold);
    shader.loadUniformLocation(AlgineNames::BloomSearchShader::BaseImage);
}

void loadBlurShaderLocations(ShaderProgram &shader) {
    shader.loadAttribLocation(AlgineNames::QuadShader::InPos);
    shader.loadAttribLocation(AlgineNames::QuadShader::InTexCoord);

    // fragment shader
    shader.loadUniformLocation(AlgineNames::BlurShader::BaseImage);
    shader.loadUniformLocation(AlgineNames::BlurShader::Kernel);
}

void loadCubemapShaderLocations(ShaderProgram &shader) {
    using namespace AlgineNames::CubemapShader;

    shader.loadAttribLocation(InPos);
    shader.loadUniformLocation(TransformationMatrix);

    // fragment shader
    shader.loadUniformLocation(CubemapTex);
    shader.loadUniformLocation(Color);
    shader.loadUniformLocation(PosScaling);
    shader.loadUniformLocation(ViewMatrix);
}

} // namespace scompiler
} // namespace algine

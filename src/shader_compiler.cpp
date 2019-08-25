#include <algine/shader_compiler.h>
#include <algine/shader_program.h>
#include <algine/sconstants.h>
#include <algine/io.h>
#include <algine/core_utils.h>

namespace algine {
namespace scompiler { // shader compiler
ShadersData readShader(const ShadersPaths &path) {
    char
        *vertexSource = io::read(path.vertex.c_str()),
        *fragmentSource = io::read(path.fragment.c_str()),
        *geometrySource = nullptr;

    if (path.geometry != "") geometrySource = io::read(path.geometry.c_str());

    ShadersData shader = ShadersData { vertexSource, fragmentSource, geometrySource == nullptr ? "" : geometrySource };

    delete[] vertexSource;
    delete[] fragmentSource;
    if (geometrySource == nullptr) delete[] geometrySource;

    return shader;
}

#define _(chars) std::string(chars)

// color shader
ShadersData getCS(const AlgineParams &params, const ColorShaderParams &csp, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData cs = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    
    cs.vertex = replace(cs.vertex, ALGINE_SHADER_DEFINITIONS,
        _(params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
        params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
        "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n") + 
        
        _(params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
        "#define ALGINE_BONE_SYSTEM_DISABLED\n") +
            
        "#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(csp.maxBoneAttribsPerVertex) + "\n" +
        "#define MAX_BONES " + std::to_string(csp.maxBones) + "\n"
    );
    
    cs.fragment = replace(cs.fragment, ALGINE_SHADER_DEFINITIONS,
        _(params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
        params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
        "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n") +
        
        _(params.shadowMappingMode == ALGINE_SHADOW_MAPPING_MODE_ENABLED ? "#define ALGINE_SHADOW_MAPPING_MODE_ENABLED\n" :
        params.shadowMappingMode == ALGINE_SHADOW_MAPPING_MODE_SIMPLE ? "#define ALGINE_SHADOW_MAPPING_MODE_SIMPLE\n" : 
        "#define ALGINE_SHADOW_MAPPING_MODE_DISABLED\n") +
        
        _(params.textureMappingMode == ALGINE_TEXTURE_MAPPING_MODE_ENABLED ? "#define ALGINE_TEXTURE_MAPPING_MODE_ENABLED\n" :
        params.textureMappingMode == ALGINE_TEXTURE_MAPPING_MODE_DUAL ? "#define ALGINE_TEXTURE_MAPPING_MODE_DUAL\n" : 
        "#define ALGINE_TEXTURE_MAPPING_MODE_DISABLED\n") +
        
        _(params.lightingMode == ALGINE_LIGHTING_MODE_ENABLED ? "#define ALGINE_LIGHTING_MODE_ENABLED\n" :
        "#define ALGINE_LIGHTING_MODE_DISABLED\n") +
        
        _(params.attenuationMode == ALGINE_ATTENUATION_MODE_ENABLED ? "#define ALGINE_ATTENUATION_MODE_ENABLED\n" :
        "#define ALGINE_ATTENUATION_MODE_DISABLED\n") +
        
        _(params.ssrMode == ALGINE_SSR_MODE_ENABLED ? "#define ALGINE_SSR_MODE_ENABLED\n" :
        "#define ALGINE_SSR_MODE_DISABLED\n") +
        
        ("#define MAX_POINT_LIGHTS_COUNT " + std::to_string(csp.maxPointLightsCount) + "\n" +
        "#define MAX_DIR_LIGHTS_COUNT " + std::to_string(csp.maxDirLightsCount) + "\n")
    );
    
    return cs;
}

// shadow shader
ShadersData getSS(const AlgineParams &params, const ShadowShaderParams &ssp, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath) {
    ShadersData ss;
    if (geometryShaderPath != nullptr)
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath, geometryShaderPath });
    else
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    
    ss.vertex = replace(ss.vertex, ALGINE_SHADER_DEFINITIONS,
        _(params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
        "#define ALGINE_BONE_SYSTEM_DISABLED\n") +
        
        _(params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
        "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n") +
        
        _("#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(ssp.maxBoneAttribsPerVertex) + "\n" +
        "#define MAX_BONES " + std::to_string(ssp.maxBones) + "\n")
    );

    ss.fragment = replace(ss.fragment, ALGINE_SHADER_DEFINITIONS,
        _(params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
        "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n")
    );
    
    return ss;
}

// dof blur shader
std::vector<ShadersData> getDOFBlurShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData dof = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData resultHorizontal, resultVertical;

    resultVertical.vertex = resultHorizontal.vertex = dof.vertex;
    
    std::string replacement =
        _(params.type == ALGINE_CINEMATIC_DOF ? "#define ALGINE_CINEMATIC_DOF\n" :
        params.type == ALGINE_LINEAR_DOF ? "#define ALGINE_LINEAR_DOF\n" :
        params.type == ALGINE_DOF_FROM_COC_MAP ? "#define ALGINE_DOF_FROM_COC_MAP\n" : "") +
        
        _(params.bleedingEliminationDeltaZ == ALGINE_ENABLED ? "#define ALGINE_BLEEDING_ELIM_DZ\n" : "") +
        _(params.bleedingEliminationDeltaCoC == ALGINE_ENABLED ? "#define ALGINE_BLEEDING_ELIM_DCOC\n" : "") +
        _(params.bleedingEliminationFocusCoC == ALGINE_ENABLED ? "#define ALGINE_BLEEDING_ELIM_FCOC\n" : "") +
        
        "#define KERNEL_RADIUS " + std::to_string(params.blurKernelRadius) + "\n";

    // fragment shader vertical
    resultVertical.fragment = replace(dof.fragment, ALGINE_SHADER_DEFINITIONS, replacement);
    
    // fragment shader horizontal 2nd part
    replacement += "#define ALGINE_HORIZONTAL\n";
    resultHorizontal.fragment = replace(dof.fragment, ALGINE_SHADER_DEFINITIONS, replacement);

    return std::vector<ShadersData> { resultHorizontal, resultVertical };
}

// dof coc shader
ShadersData getDOFCoCShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData sdata = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    
    sdata.fragment = replace(sdata.fragment, ALGINE_SHADER_DEFINITIONS,
        _(params.type == ALGINE_CINEMATIC_DOF ? "#define ALGINE_CINEMATIC_DOF\n" :
        "#define ALGINE_LINEAR_DOF\n")
    );

    return sdata;
}

// blend shader
ShadersData getBlendShader(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData bles = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    
    bles.fragment = replace(bles.fragment, ALGINE_SHADER_DEFINITIONS,
        _(params.bloomMode == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_ENABLED\n" : 
        "#define ALGINE_BLOOM_DISABLED\n") +
        
        _(params.bloomType == ALGINE_BLOOM_TYPE_ADD ? "#define ALGINE_BLOOM_TYPE_ADD\n" : 
        "#define ALGINE_BLOOM_TYPE_SCREEN\n")
    );
    
    return bles;
}

// screen space reflections shader
ShadersData getSSRShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
}

// bloom search shader
ShadersData getBloomSearchShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
}

// returns `std::vector<ShadersData> { blurHorizontal, blurVertical }`
std::vector<ShadersData> getBlurShader(const BlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData blur = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData resultHorizontal, resultVertical;

    resultHorizontal.vertex = resultVertical.vertex = blur.vertex;
    
    std::string replacement =
        "#define KERNEL_RADIUS " + std::to_string(params.blurKernelRadius) + "\n" +
        "#define vecout " + getShaderStr(params.vecout) + "\n" +
        "#define texComponent " + getShaderStr(params.texComponent) + "\n";

    resultHorizontal.fragment = replace(blur.fragment, ALGINE_SHADER_DEFINITIONS,
        replacement + "#define ALGINE_HORIZONTAL\n"
    );
    
    resultVertical.fragment = replace(blur.fragment, ALGINE_SHADER_DEFINITIONS,
        replacement + "#define ALGINE_VERTICAL\n"
    );

    // in alphabet order
    return std::vector<ShadersData> { resultHorizontal, resultVertical };
}

// cubemap shader
ShadersData getCubemapShader(const CubemapShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData shaderfile = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    
    shaderfile.fragment = replace(shaderfile.fragment, ALGINE_SHADER_DEFINITIONS,
        "#define ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT " + std::to_string(params.cubemapColorOutColorComponent) + "\n" +
        "#define ALGINE_POS_OUT_COLOR_COMPONENT " + std::to_string(params.positionOutColorComponent) + "\n" +
        "#define vecout " + getShaderStr(params.vecout) + "\n" +
        
        (params.cubemapColorOutput == ALGINE_ENABLED ? "#define ALGINE_CUBEMAP_COLOR\n" : "") +
        
        (params.positionOutput == ALGINE_CUBE_POSITIONS ? "#define ALGINE_CUBE_POSITIONS\n" :
        params.positionOutput == ALGINE_SPHERE_POSITIONS ? "#define ALGINE_SPHERE_POSITIONS\n" : "")
    );
    
    return shaderfile;
}

void saveShaders(const ShadersData &shader, const ShadersPaths &path) {
    FILE *file = nullptr;
    io::save(file, shader.vertex, path.vertex);
    io::save(file, shader.fragment, path.fragment);
    if (!shader.geometry.empty())
        io::save(file, shader.geometry, path.geometry);
}

void loadColorShaderLocations(ShaderProgram &shader, const ColorShaderParams &csp) {
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
    for (usize i = 0; i < csp.maxPointLightsCount; i++) {
        start = PointLights + std::string("[") + std::to_string(i) + "].";
        loadLightProp
        shader.loadUniformLocation(start + Light::FarPlane);
        shader.loadUniformLocation(start + Light::Bias);
        shader.loadUniformLocation(PointLightShadowMaps + std::string("[") + std::to_string(i) + "]");
    }

    // directional lights
    for (usize i = 0; i < csp.maxDirLightsCount; i++) {
        start = DirLights + std::string("[") + std::to_string(i) + "].";
        loadLightProp
        shader.loadUniformLocation(start + Light::LightMatrix);
        shader.loadUniformLocation(start + Light::MinBias);
        shader.loadUniformLocation(start + Light::MaxBias);
        shader.loadUniformLocation(DirLightShadowMaps + std::string("[") + std::to_string(i) + "]");
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

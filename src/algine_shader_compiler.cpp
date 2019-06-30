#ifndef ALGINE_SHADER_COMPILER_CPP
#define ALGINE_SHADER_COMPILER_CPP

#include <string>
#include <vector>
#include "algine_structs.cpp"
#include "shaderprogram.cpp"
#include <algine/sconstants.h>
#include <algine/io.h>
#include <algine/core_utils.h>

namespace algine {
// shader compiler
namespace scompiler {
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

// color shader
ShadersData getCS(const AlgineParams &params, const ColorShaderParams &csp, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData cs = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    std::vector<std::string> out = split(cs.vertex, ALGINE_SHADER_DEFINITIONS);
    result.vertex = 
        out[0] + (
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + (
            params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
            "#define ALGINE_BONE_SYSTEM_DISABLED\n"
        ) + (
            "#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(csp.maxBoneAttribsPerVertex) + "\n" +
            "#define MAX_BONES " + std::to_string(csp.maxBones) + "\n"
        ) + out[1];

    out = split(cs.fragment, ALGINE_SHADER_DEFINITIONS);
    result.fragment = 
        out[0] + (
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + (
            params.shadowMappingMode == ALGINE_SHADOW_MAPPING_MODE_ENABLED ? "#define ALGINE_SHADOW_MAPPING_MODE_ENABLED\n" :
            params.shadowMappingMode == ALGINE_SHADOW_MAPPING_MODE_SIMPLE ? "#define ALGINE_SHADOW_MAPPING_MODE_SIMPLE\n" : 
            "#define ALGINE_SHADOW_MAPPING_MODE_DISABLED\n"
        ) + (
            params.textureMappingMode == ALGINE_TEXTURE_MAPPING_MODE_ENABLED ? "#define ALGINE_TEXTURE_MAPPING_MODE_ENABLED\n" :
            params.textureMappingMode == ALGINE_TEXTURE_MAPPING_MODE_DUAL ? "#define ALGINE_TEXTURE_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_TEXTURE_MAPPING_MODE_DISABLED\n"
        ) + (
            params.lightingMode == ALGINE_LIGHTING_MODE_ENABLED ? "#define ALGINE_LIGHTING_MODE_ENABLED\n" :
            "#define ALGINE_LIGHTING_MODE_DISABLED\n"
        ) + (
            params.attenuationMode == ALGINE_ATTENUATION_MODE_ENABLED ? "#define ALGINE_ATTENUATION_MODE_ENABLED\n" :
            "#define ALGINE_ATTENUATION_MODE_DISABLED\n"
        ) + (
            params.ssrMode == ALGINE_SSR_MODE_ENABLED ? "#define ALGINE_SSR_MODE_ENABLED\n" :
            "#define ALGINE_SSR_MODE_DISABLED\n"
        ) + (
            "#define MAX_POINT_LIGHTS_COUNT " + std::to_string(csp.maxPointLightsCount) + "\n" +
            "#define MAX_DIR_LIGHTS_COUNT " + std::to_string(csp.maxDirLightsCount) + "\n"
        ) + out[1];
        
    return result;
}

// shadow shader
ShadersData getSS(const AlgineParams &params, const ShadowShaderParams &ssp, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr) {
    ShadersData ss;
    if (geometryShaderPath != nullptr)
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath, geometryShaderPath });
    else
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    std::vector<std::string> out = split(ss.vertex, ALGINE_SHADER_DEFINITIONS);
    result.vertex =
        out[0] + (
            params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
            "#define ALGINE_BONE_SYSTEM_DISABLED\n"
        ) + (
            params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
            "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n"
        ) + (
            "#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(ssp.maxBoneAttribsPerVertex) + "\n" +
            "#define MAX_BONES " + std::to_string(ssp.maxBones) + "\n"
        ) + out[1];

    out = split(ss.fragment, ALGINE_SHADER_DEFINITIONS);
    result.fragment =
        out[0] + (
            params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
            "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n"
        ) + out[1];

    if (params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING)
        result.geometry = ss.geometry;
    
    return result;
}

// dof blur shader
#define s(n) std::string(n)
std::vector<ShadersData> getDOFBlurShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData dof = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData resultHorizontal, resultVertical;

    resultVertical.vertex = resultHorizontal.vertex = dof.vertex;

    std::vector<std::string> out = split(dof.fragment, ALGINE_SHADER_DEFINITIONS);

    resultHorizontal.fragment = 
        out[0] + (
            params.type == ALGINE_CINEMATIC_DOF ? "#define ALGINE_CINEMATIC_DOF\n" :
            params.type == ALGINE_LINEAR_DOF ? "#define ALGINE_LINEAR_DOF\n" :
            params.type == ALGINE_DOF_FROM_COC_MAP ? "#define ALGINE_DOF_FROM_COC_MAP\n" : ""
        ) + (
            (params.bleedingEliminationDeltaZ == ALGINE_ENABLED ? s("#define ALGINE_BLEEDING_ELIM_DZ\n") : s()) +
            (params.bleedingEliminationDeltaCoC == ALGINE_ENABLED ? s("#define ALGINE_BLEEDING_ELIM_DCOC\n") : s()) +
            (params.bleedingEliminationFocusCoC == ALGINE_ENABLED ? s("#define ALGINE_BLEEDING_ELIM_FCOC\n") : s())
        ) + (
            "#define KERNEL_RADIUS " + std::to_string(params.blurKernelRadius) + "\n"
        );

    // fragment shader vertical 1st and 2nd part
    resultVertical.fragment = resultHorizontal.fragment + out[1];
    // fragment shader horizontal 2nd part
    resultHorizontal.fragment += "#define ALGINE_HORIZONTAL\n" + out[1];

    return std::vector<ShadersData> { resultHorizontal, resultVertical };
}
#undef s

// dof coc shader
ShadersData getDOFCoCShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData sdata = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    result.vertex = sdata.vertex;

    std::vector<std::string> out = split(sdata.fragment, ALGINE_SHADER_DEFINITIONS);

    result.fragment = 
        out[0] + (
            params.type == ALGINE_CINEMATIC_DOF ? "#define ALGINE_CINEMATIC_DOF\n" :
            "#define ALGINE_LINEAR_DOF\n"
        ) + out[1];

    return result;
}

// blend shader
ShadersData getBlendShader(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData bles = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    result.vertex = bles.vertex;

    std::vector<std::string> out = split(bles.fragment, ALGINE_SHADER_DEFINITIONS);

    result.fragment = 
        out[0] + (
            params.bloomMode == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_ENABLED\n" : 
            "#define ALGINE_BLOOM_DISABLED\n"
        ) + (
            params.bloomType == ALGINE_BLOOM_TYPE_ADD ? "#define ALGINE_BLOOM_TYPE_ADD\n" : 
            "#define ALGINE_BLOOM_TYPE_SCREEN\n"
        ) + out[1];

    return result;
}

// screen space reflections shader
ShadersData getSSRShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
}

ShadersData getBloomSearchShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
}

// returns `std::vector<ShadersData> { blurHorizontal, blurVertical }`
std::vector<ShadersData> getBlurShader(const BlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData blur = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData resultHorizontal, resultVertical;

    resultHorizontal.vertex = resultVertical.vertex = blur.vertex;

    std::vector<std::string> out = split(blur.fragment, ALGINE_SHADER_DEFINITIONS);

    resultHorizontal.fragment = 
        out[0] + (
            "#define KERNEL_RADIUS " + std::to_string(params.blurKernelRadius) + "\n" +
            "#define vecout " + getShaderStr(params.vecout) + "\n" +
            "#define texComponent " + getShaderStr(params.texComponent) + "\n"
        );

    resultVertical.fragment = resultHorizontal.fragment + "#define ALGINE_VERTICAL\n" + out[1];
    resultHorizontal.fragment += "#define ALGINE_HORIZONTAL\n" + out[1];

    // in alphabet order
    return std::vector<ShadersData> { resultHorizontal, resultVertical };
}

ShadersData getCubemapShader(const CubemapShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData shaderfile = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    result.vertex = shaderfile.vertex;

    std::vector<std::string> out = split(shaderfile.fragment, ALGINE_SHADER_DEFINITIONS);

    result.fragment = 
        out[0] + (
            "#define ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT " + std::to_string(params.cubemapColorOutColorComponent) + "\n" +
            "#define ALGINE_POS_OUT_COLOR_COMPONENT " + std::to_string(params.positionOutColorComponent) + "\n"
        ) + (
            "#define vecout " + getShaderStr(params.vecout) + "\n"
        ) + (
            params.cubemapColorOutput == ALGINE_ENABLED ? "#define ALGINE_CUBEMAP_COLOR\n" : ""
        ) + (
            params.positionOutput == ALGINE_CUBE_POSITIONS ? "#define ALGINE_CUBE_POSITIONS\n" :
            params.positionOutput == ALGINE_SPHERE_POSITIONS ? "#define ALGINE_SPHERE_POSITIONS\n" : ""
        ) + out[1];

    return result;
}

void saveShaders(const ShadersData &shader, const ShadersPaths &path) {
    FILE *file = nullptr;
    io::save(file, shader.vertex, path.vertex);
    io::save(file, shader.fragment, path.fragment);
    if (shader.geometry != "") io::save(file, shader.geometry, path.geometry);
}

GLuint compileShader(const std::string &shader, const GLuint type) {
    GLuint s = Shader::create(type);
    Shader::compile(s, shader);
    return s;
}

// compiles vertex & fragment & geometry (if exists) shaders and 
std::vector<GLuint> compileShaders(const ShadersData &shader) {
    std::vector<GLuint> result;

    result.push_back(compileShader(shader.vertex, ALGINE_VERTEX_SHADER));
    result.push_back(compileShader(shader.fragment, ALGINE_FRAGMENT_SHADER));
    if (shader.geometry != "") result.push_back(compileShader(shader.geometry, ALGINE_GEOMETRY_SHADER));

    return result;
}

GLuint createShaderProgram(const std::vector<GLuint> &shaders) {
    GLuint result = ShaderProgram::create();
    for (size_t i = 0; i < shaders.size(); i++) ShaderProgram::attachShader(result, shaders[i]);
    ShaderProgram::link(result);
    return result;
}

// DS - delete shaders
GLuint createShaderProgramDS(const std::vector<GLuint> &shaders) {
    GLuint sp = createShaderProgram(shaders);
    for (size_t i = 0; i < shaders.size(); i++) Shader::destroy(shaders[i]);
    return sp;
}

void loadLocations(CShader &shader, const AlgineParams &params, const ColorShaderParams &csp) {
    shader.matModel = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MAT_MODEL);
    shader.matView = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MAT_VIEW);
    shader.matPVM = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MAT_PVM);
    shader.matVM = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MAT_VM);
    shader.bones = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_BONES);
    shader.normalMappingSwitcher = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_SWITCH_NORMAL_MAPPING);
    shader.boneAttribsPerVertex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_BONE_ATTRIBS_PER_VERTEX);
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_POSITION);
    shader.inNormal = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_NORMAL);
    shader.inTangent = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_TANGENT);
    shader.inBitangent = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_BITANGENT);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_TEXCOORD);
    shader.inBoneIds = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_BONE_IDS);
    shader.inBoneWeights = glGetAttribLocation(shader.programId, ALGINE_NAME_CS_IN_BONE_WEIGHTS);
    
    // fragment shader
    shader.viewPos = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_VIEW_POSITION);
    shader.pointLightsCount = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_POINT_LIGHTS_COUNT);
    shader.dirLightsCount = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_DIR_LIGHTS_COUNT);
    shader.shadowDiskRadiusK = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_SHADOW_DISKRADIUS_K);
    shader.shadowDiskRadiusMin = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN);
    shader.shadowOpacity = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_SHADOW_OPACITY);
    // material
    shader.materialAmbientTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_AMBIENT_TEX);
    shader.materialDiffuseTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_DIFFUSE_TEX);
    shader.materialSpecularTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_SPECULAR_TEX);
    shader.materialNormalTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_NORMAL_TEX);
    shader.materialReflectionStrengthTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_REFLECTIONSTRENGTH_TEX);
    shader.materialJitterTex = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_JITTER_TEX);
    shader.materialAmbientColor = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_AMBIENT_COLOR);
    shader.materialDiffuseColor = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_DIFFUSE_COLOR);
    shader.materialSpecularColor = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_SPECULAR_COLOR);
    shader.materialAmbientStrength = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_AMBIENT_STRENGTH);
    shader.materialDiffuseStrength = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_DIFFUSE_STRENGTH);
    shader.materialSpecularStrength = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_SPECULAR_STRENGTH);
    shader.materialShininess = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_MATERIAL_SHININESS);
    // mapping
    shader.textureMappingSwitcher = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_SWITCH_TEXTURE_MAPPING);
    
    // lights
    shader.pointLights.reserve(csp.maxPointLightsCount);
    shader.dirLights.reserve(csp.maxDirLightsCount);

    #define loadLightProp(light) \
        shader.light.kc = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_KC).c_str()); \
        shader.light.kl = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_KL).c_str()); \
        shader.light.kq = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_KQ).c_str()); \
        shader.light.pos = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_POS).c_str()); \
        shader.light.color = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_COLOR).c_str()); \
        shader.light.shadowMap = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_SHADOW_MAP).c_str());

    // uns - uniform name start
    std::string uns;

    // point lights
    for (usize i = 0; i < csp.maxPointLightsCount; i++) {
        uns = ALGINE_NAME_CS_POINT_LIGHTS + std::string("[") + std::to_string(i) + "].";
        shader.pointLights.push_back(PointLightIds());
        loadLightProp(pointLights[i]);
        shader.pointLights[i].far = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_FAR).c_str());
        shader.pointLights[i].bias = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_BIAS).c_str());
    }

    // directional lights
    for (usize i = 0; i < csp.maxDirLightsCount; i++) {
        uns = ALGINE_NAME_CS_DIR_LIGHTS + std::string("[") + std::to_string(i) + "].";
        shader.dirLights.push_back(DirLightIds());
        loadLightProp(dirLights[i]);
        shader.dirLights[i].lightMatrix = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_MATRIX).c_str());
        shader.dirLights[i].minBias = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_MIN_BIAS).c_str());
        shader.dirLights[i].maxBias = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_MAX_BIAS).c_str());
    }

    #undef loadLightProp
}

void loadLocations(SShader &shader) {
    // geometry shader
    shader.shadowMatrices = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_MAT_SHADOW);

    // vertex shader
    shader.matLightSpace = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_MAT_LIGHT_SPACE);
    shader.matModel = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_MAT_MODEL);
    shader.bones = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_BONES);
    shader.boneAttribsPerVertex = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_BONE_ATTRIBS_PER_VERTEX);
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_SS_IN_POSITION);
    shader.inBoneIds = glGetAttribLocation(shader.programId, ALGINE_NAME_SS_IN_BONE_IDS);
    shader.inBoneWeights = glGetAttribLocation(shader.programId, ALGINE_NAME_SS_IN_BONE_WEIGHTS);
    
    // fragment shader
    shader.lampPos = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_LAMP_POSITION);
    shader.far = glGetUniformLocation(shader.programId, ALGINE_NAME_SS_FAR_PLANE);
}

#define _loadDOFLocations(shader) \
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_DOF_IN_POSITION); \
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_DOF_IN_TEXCOORD); \
    \
    /* fragment shader */ \
    shader.samplerPositionMap = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_SAMPLER_MAP_POSITION); \
    /* linear DOF parameters */ \
    shader.focalDepth = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_FOCAL_DEPTH); \
    shader.focalRange = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_FOCAL_RANGE); \
    shader.sigmaMin = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_SIGMA_MIN); \
    shader.sigmaMax = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_SIGMA_MAX); \
    /* cinematic DOF parameters */ \
    shader.cinematicDOFPlaneInFocus = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_CINEMATIC_DOF_PLANE_IN_FOCUS); \
    shader.cinematicDOFAperture = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_CINEMATIC_DOF_APERTURE); \
    shader.cinematicDOFImageDistance = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_CINEMATIC_DOF_IMAGE_DISTANCE);

void loadLocations(DOFBlurShader &shader) {
    _loadDOFLocations(shader)
    
    // fragment shader
    shader.samplerImage = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_SAMPLER_IMAGE);
    shader.samplerCoCMap = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_SAMPLER_MAP_COC);
    shader.bleedingEliminationMinDeltaZ = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_BLEEDING_ELIM_MIN_DZ);
    shader.bleedingEliminationMinDeltaCoC = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_BLEEDING_ELIM_MIN_DCOC);
    shader.bleedingEliminationMaxFocusCoC = glGetUniformLocation(shader.programId, ALGINE_NAME_DOF_BLEEDING_ELIM_MAX_FCOC);
}

void loadLocations(DOFCoCShader &shader) {
    _loadDOFLocations(shader)
}

#undef _loadDOFLocations

void loadLocations(BlendShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_BLEND_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_BLEND_IN_TEXCOORD);

    // fragment shader
    shader.samplerBloom = glGetUniformLocation(shader.programId, ALGINE_NAME_BLEND_SAMPLER_BLOOM);
    shader.samplerImage = glGetUniformLocation(shader.programId, ALGINE_NAME_BLEND_SAMPLER_IMAGE);
    shader.exposure = glGetUniformLocation(shader.programId, ALGINE_NAME_BLEND_EXPOSURE);
    shader.gamma = glGetUniformLocation(shader.programId, ALGINE_NAME_BLEND_GAMMA);
}

void loadLocations(SSRShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_SSRS_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_SSRS_IN_TEXCOORD);

    // SSS fragment shader
    shader.samplerColorMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_SAMPLER_MAP_COLOR);
    shader.samplerNormalMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_SAMPLER_MAP_NORMAL);
    shader.samplerSSRValuesMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_SAMPLER_MAP_SSRVALUES);
    shader.samplerPositionMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_SAMPLER_MAP_POSITION);
    shader.matProjection = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_MAT_PROJECTION);
    shader.matView = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_MAT_VIEW);
    shader.skyColor = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_SKYCOLOR);
    shader.binarySearchCount = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_BINARYSEARCHCOUNT);
    shader.rayMarchCount = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_RAYMARCHCOUNT);
    shader.step = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_STEP);
    shader.LLimiter = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_LLIMITER);
    shader.minRayStep = glGetUniformLocation(shader.programId, ALGINE_NAME_SSRS_MINRAYSTEP);
}

void loadLocations(BloomSearchShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_BLOOM_SEARCH_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_BLOOM_SEARCH_IN_TEXCOORD);

    // fragment shader
    shader.threshold = glGetUniformLocation(shader.programId, ALGINE_NAME_BLOOM_SEARCH_THRESHOLD);
    shader.brightnessThreshold = glGetUniformLocation(shader.programId, ALGINE_NAME_BLOOM_SEARCH_BRIGHTNESS_THRESHOLD);
    shader.image = glGetUniformLocation(shader.programId, ALGINE_NAME_BLOOM_SEARCH_IMAGE);
}

void loadLocations(BlurShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_BLUR_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_BLUR_IN_TEXCOORD);

    // fragment shader
    shader.image = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUR_IMAGE);
    shader.kernel = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUR_KERNEL);
}

void loadLocations(CubemapShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_CUBEMAP_IN_POSITION);
    shader.matTransform = glGetUniformLocation(shader.programId, ALGINE_NAME_CUBEMAP_MAT_TRANSFORM);

    // fragment shader
    shader.cubemap = glGetUniformLocation(shader.programId, ALGINE_NAME_CUBEMAP_CUBEMAP);
    shader.color = glGetUniformLocation(shader.programId, ALGINE_NAME_CUBEMAP_COLOR);
    shader.positionScaling = glGetUniformLocation(shader.programId, ALGINE_NAME_CUBEMAP_POS_SCALING);
}

} // namespace scompiler
} // namespace algine

#endif /* ALGINE_SHADER_COMPILER_CPP */

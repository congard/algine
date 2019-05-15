#ifndef ALGINE_SHADER_COMPILER_CPP
#define ALGINE_SHADER_COMPILER_CPP

#include <string>
#include <vector>
#include "algine_structs.cpp"
#include "shaderprogram.cpp"
#include <algine/sconstants.h>
#include <algine/io.h>

namespace algine {
// shader compiler
namespace scompiler {
static std::string ALGINE_DEF_DELIMITER = "#algdef";

void splitByDelimiter(std::vector<std::string> &out, std::string &in) {
    size_t pos = 0;
    std::string token;
    while ((pos = in.find(ALGINE_DEF_DELIMITER)) != std::string::npos) {
        token = in.substr(0, pos);
        out.push_back(token);
        in.erase(0, pos + ALGINE_DEF_DELIMITER.length());
    }
    out.push_back(in);
}

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
ShadersData getCS(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData cs = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    std::vector<std::string> out;
    splitByDelimiter(out, cs.vertex);
    result.vertex = 
        out[0] + (
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            params.normalMappingMode == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + (
            params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
            "#define ALGINE_BONE_SYSTEM_DISABLED\n"
        ) + (
            "#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(params.maxBoneAttribsPerVertex) + "\n" +
            "#define MAX_BONES " + std::to_string(params.maxBones) + "\n"
        ) + out[1];

    out.clear();
    splitByDelimiter(out, cs.fragment);
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
            params.dofMode == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" :
            params.dofMode == ALGINE_CINEMATIC_DOF_MODE_ENABLED ? "#define ALGINE_CINEMATIC_DOF_MODE_ENABLED\n" :
            "#define ALGINE_DOF_MODE_DISABLED\n"
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
            "#define MAX_POINT_LIGHTS_COUNT " + std::to_string(params.maxPointLightsCount) + "\n" +
            "#define MAX_DIR_LIGHTS_COUNT " + std::to_string(params.maxDirLightsCount) + "\n"
        ) + out[1];
        
    return result;
}

// shadow shader
ShadersData getSS(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr) {
    ShadersData ss;
    if (geometryShaderPath != nullptr)
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath, geometryShaderPath });
    else
        ss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    std::vector<std::string> out;
    splitByDelimiter(out, ss.vertex);

    result.vertex =
        out[0] + (
            params.boneSystemMode == ALGINE_BONE_SYSTEM_ENABLED ? "#define ALGINE_BONE_SYSTEM_ENABLED\n" :
            "#define ALGINE_BONE_SYSTEM_DISABLED\n"
        ) + (
            params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
            "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n"
        ) + (
            "#define MAX_BONE_ATTRIBS_PER_VERTEX " + std::to_string(params.maxBoneAttribsPerVertex) + "\n" +
            "#define MAX_BONES " + std::to_string(params.maxBones) + "\n"
        ) + out[1];

    out.clear();
    splitByDelimiter(out, ss.fragment);
    result.fragment =
        out[0] + (
            params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING ? "#define ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING\n" :
            "#define ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING\n"
        ) + out[1];

    if (params.shadowMappingType == ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING)
        result.geometry = ss.geometry;
    
    return result;
}

// blur shader
std::vector<ShadersData> getBLUS(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData blus = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData resultHorizontal, resultVertical;

    resultVertical.vertex = resultHorizontal.vertex = blus.vertex;

    std::vector<std::string> out;
    splitByDelimiter(out, blus.fragment);

    resultHorizontal.fragment = 
        out[0] + (
            params.bloomMode == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n#define BLOOM_KERNEL_SIZE " + std::to_string(params.bloomKernelSize) + "\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.dofMode == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" :
            params.dofMode == ALGINE_CINEMATIC_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n#define ALGINE_CINEMATIC_DOF_MODE_ENABLED\n" :
            "#define ALGINE_DOF_MODE_DISABLED\n"
        ) + (
            params.dofMode == ALGINE_DOF_MODE_ENABLED || params.dofMode == ALGINE_CINEMATIC_DOF_MODE_ENABLED ?
                "#define DOF_KERNEL_SIZE " + std::to_string(params.dofKernelSize) + "\n" : ""
        );

    // fragment shader vertical 1st and 2nd part
    resultVertical.fragment = resultHorizontal.fragment + out[1];
    // fragment shader horizontal 2nd part
    resultHorizontal.fragment += "#define ALGINE_BLUS_HORIZONTAL\n" + out[1];

    return std::vector<ShadersData> { resultHorizontal, resultVertical };
}

// blend shader
ShadersData getBLES(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData bles = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    result.vertex = bles.vertex;

    std::vector<std::string> out;
    splitByDelimiter(out, bles.fragment);

    result.fragment = 
        out[0] + (
            params.bloomMode == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.dofMode == ALGINE_DOF_MODE_ENABLED || params.dofMode == ALGINE_CINEMATIC_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" : 
            "#define ALGINE_DOF_MODE_DISABLED\n"
        ) + out[1];

    return result;
}

// screen space shader
ShadersData getSSS(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath) {
    ShadersData sss = readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath });
    ShadersData result;

    result.vertex = sss.vertex;

    std::vector<std::string> out;
    splitByDelimiter(out, sss.fragment);

    result.fragment = 
        out[0] + (
            params.bloomMode == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.ssrMode == ALGINE_SSR_MODE_ENABLED ? "#define ALGINE_SSR_MODE_ENABLED\n" : 
            "#define ALGINE_SSR_MODE_DISABLED\n"
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

void loadLocations(CShader &shader, const AlgineParams &params) {
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
    // linear DOF parameters
    shader.focalDepth = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_FOCAL_DEPTH);
    shader.focalRange = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_FOCAL_RANGE);
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
    // cinematic DOF parameters
    shader.cinematicDOFPlaneInFocus = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_CINEMATIC_DOF_PLANE_IN_FOCUS);
    shader.cinematicDOFAperture = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_CINEMATIC_DOF_APERTURE);
    shader.cinematicDOFImageDistance = glGetUniformLocation(shader.programId, ALGINE_NAME_CS_CINEMATIC_DOF_IMAGE_DISTANCE);
    
    // lights
    shader.pointLights.reserve(params.maxPointLightsCount);
    shader.dirLights.reserve(params.maxDirLightsCount);

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
    for (usize i = 0; i < params.maxPointLightsCount; i++) {
        uns = ALGINE_NAME_CS_POINT_LIGHTS + std::string("[") + std::to_string(i) + "].";
        shader.pointLights.push_back(PointLightIds());
        loadLightProp(pointLights[i]);
        shader.pointLights[i].far = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_FAR).c_str());
        shader.pointLights[i].bias = glGetUniformLocation(shader.programId, (uns + ALGINE_NAME_CS_LIGHT_BIAS).c_str());
    }

    // directional lights
    for (usize i = 0; i < params.maxDirLightsCount; i++) {
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

void loadLocations(BLUShader &shader, const AlgineParams &params) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_BLUS_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_BLUS_IN_TEXCOORD);
    
    // fragment shader
    shader.samplerBloom = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_SAMPLER_BLOOM);
    shader.samplerScene = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_SAMPLER_SCENE);
    shader.samplerDofBuffer = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_SAMPLER_DOF_BUFFER);
    shader.sigmaMin = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_SIGMA_MIN);
    shader.sigmaMax = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_SIGMA_MAX);
    shader.bloomKernel = glGetUniformLocation(shader.programId, ALGINE_NAME_BLUS_KERNEL_BLOOM);
}

void loadLocations(BLEShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_BLES_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_BLES_IN_TEXCOORD);

    // fragment shader
    shader.samplerBloomScene = glGetUniformLocation(shader.programId, ALGINE_NAME_BLES_SAMPLER_BLOOM_SCENE);
    shader.samplerDofScene = glGetUniformLocation(shader.programId, ALGINE_NAME_BLES_SAMPLER_DOF_SCENE);
    shader.exposure = glGetUniformLocation(shader.programId, ALGINE_NAME_BLES_EXPOSURE);
    shader.gamma = glGetUniformLocation(shader.programId, ALGINE_NAME_BLES_GAMMA);
}

void loadLocations(SSShader &shader) {
    shader.inPosition = glGetAttribLocation(shader.programId, ALGINE_NAME_SSS_IN_POSITION);
    shader.inTexCoord = glGetAttribLocation(shader.programId, ALGINE_NAME_SSS_IN_TEXCOORD);

    // SSS fragment shader
    shader.samplerColorMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SAMPLER_MAP_COLOR);
    shader.samplerNormalMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SAMPLER_MAP_NORMAL);
    shader.samplerSSRValuesMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SAMPLER_MAP_SSRVALUES);
    shader.samplerPositionMap = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SAMPLER_MAP_POSITION);
    shader.matProjection = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_MAT_PROJECTION);
    shader.matView = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_MAT_VIEW);
    shader.ssrSkyColor = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_SKYCOLOR);
    shader.ssrBinarySearchCount = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_BINARYSEARCHCOUNT);
    shader.ssrRayMarchCount = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_RAYMARCHCOUNT);
    shader.ssrStep = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_STEP);
    shader.ssrLLimiter = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_LLIMITER);
    shader.ssrMinRayStep = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_SSR_MINRAYSTEP);
    shader.blVecThreshold = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_BL_VECTHRESHOLD);
    shader.blBrightnessThreshold = glGetUniformLocation(shader.programId, ALGINE_NAME_SSS_BL_BRIGHTNESSTHRESHOLD);
}

} // namespace scompiler
} // namespace algine

#endif /* ALGINE_SHADER_COMPILER_CPP */

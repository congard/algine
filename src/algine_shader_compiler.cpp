#include <iostream>
#include <vector>
#include "algine_structs.cpp"
#include "shaderprogram.cpp"
#include "sconstants.h"

namespace algine {
// shader compiler
namespace scompiler {
std::string ALGINE_DEF_DELIMITER = "#algdef";

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

char *read(const char *path) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *result = new char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(result, 1, size, file);
    fclose(file);
    result[size] = '\0';
    return result;
}

void save(FILE *file, const std::string &data, const std::string &path) {
    fwrite(
        data.c_str(),
        1, data.size(),
        file = fopen(path.c_str(), "wb")
    );

    fclose(file);
}

ShadersData readShader(const ShadersPaths &path) {
    char
        *vertexSource = read(path.vertex.c_str()),
        *fragmentSource = read(path.fragment.c_str()),
        *geometrySource = nullptr;

    if (path.geometry != "") geometrySource = read(path.geometry.c_str());

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
            params.NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            params.NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + out[1];

    out.clear();
    splitByDelimiter(out, cs.fragment);
    result.fragment = 
        out[0] + (
            params.NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            params.NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + (
            params.SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_ENABLED ? "#define ALGINE_SHADOW_MAPPING_MODE_ENABLED\n" :
            params.SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_SIMPLE ? "#define ALGINE_SHADOW_MAPPING_MODE_SIMPLE\n" : 
            "#define ALGINE_SHADOW_MAPPING_MODE_DISABLED\n"
        ) + (
            params.DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" :
            "#define ALGINE_DOF_MODE_DISABLED\n"
        ) + (
            params.TEXTURE_MAPPING_MODE == ALGINE_TEXTURE_MAPPING_MODE_ENABLED ? "#define ALGINE_TEXTURE_MAPPING_MODE_ENABLED\n" :
            params.TEXTURE_MAPPING_MODE == ALGINE_TEXTURE_MAPPING_MODE_DUAL ? "#define ALGINE_TEXTURE_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_TEXTURE_MAPPING_MODE_DISABLED\n"
        ) + (
            params.LIGHTING_MODE == ALGINE_LIGHTING_MODE_ENABLED ? "#define ALGINE_LIGHTING_MODE_ENABLED\n" :
            "#define ALGINE_LIGHTING_MODE_DISABLED\n"
        ) + (
            params.ATTENUATION_MODE == ALGINE_ATTENUATION_MODE_ENABLED ? "#define ALGINE_ATTENUATION_MODE_ENABLED\n" :
            "#define ALGINE_ATTENUATION_MODE_DISABLED\n"
        ) + (
            "#define MAX_LAMPS_COUNT " + std::to_string(params.MAX_LAMPS_COUNT) + "\n"
        ) + (
            params.SSR_MODE == ALGINE_SSR_MODE_ENABLED ? "#define ALGINE_SSR_MODE_ENABLED\n" :
            "#define ALGINE_SSR_MODE_DISABLED\n"
        ) + out[1];
        
    return result;
}

// shadow shader
ShadersData getSS(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath) {
    return readShader(ShadersPaths { vertexShaderPath, fragmentShaderPath, geometryShaderPath });
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
            params.BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n#define BLOOM_KERNEL_SIZE " + std::to_string(params.KERNEL_BLOOM_SIZE) + "\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n#define DOF_KERNEL_SIZE " + std::to_string(params.KERNEL_DOF_SIZE) + "\n" : 
            "#define ALGINE_DOF_MODE_DISABLED\n"
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
            params.BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" : 
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
            params.BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            params.SSR_MODE == ALGINE_SSR_MODE_ENABLED ? "#define ALGINE_SSR_MODE_ENABLED\n" : 
            "#define ALGINE_SSR_MODE_DISABLED\n"
        ) + out[1];

    return result;
}

void saveShaders(const ShadersData &shader, const ShadersPaths &path) {
    FILE *file;
    save(file, shader.vertex, path.vertex);
    save(file, shader.fragment, path.fragment);
    if (shader.geometry != "") save(file, shader.geometry, path.geometry);
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

void loadLocations(AlginePrograms &programs, const AlgineParams &params) {
    // CS vertex shader
    programs.CS_MAT_MODEL = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_MODEL);
    programs.CS_MAT_VIEW = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_VIEW);
    programs.CS_MAT_PVM = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_PVM);
    programs.CS_MAT_VM = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_VM);
    programs.CS_SWITCH_NORMAL_MAPPING = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SWITCH_NORMAL_MAPPING);
    programs.CS_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_IN_POSITION);
    programs.CS_IN_NORMAL = glGetAttribLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_IN_NORMAL);
    programs.CS_IN_TANGENT = glGetAttribLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_IN_TANGENT);
    programs.CS_IN_BITANGENT = glGetAttribLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_IN_BITANGENT);
    programs.CS_IN_TEXCOORD = glGetAttribLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_IN_TEXCOORD);
    
    // CS fragment shader
    programs.CS_VIEW_POSITION = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_VIEW_POSITION);
    programs.CS_LAMPS_COUNT = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_LAMPS_COUNT);
    programs.CS_BRIGHTNESS_THRESHOLD = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_BRIGHTNESS_THRESHOLD);
    programs.CS_SHADOW_FAR_PLANE = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_FAR_PLANE);
    programs.CS_SHADOW_DISKRADIUS_K = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_DISKRADIUS_K);
    programs.CS_SHADOW_DISKRADIUS_MIN = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN);
    programs.CS_SHADOW_BIAS = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_BIAS);
    programs.CS_FOCAL_DEPTH = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_FOCAL_DEPTH);
    programs.CS_FOCAL_RANGE = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_FOCAL_RANGE);
    programs.CS_SAMPLER_MAPPING_AMBIENT = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_AMBIENT);
    programs.CS_SAMPLER_MAPPING_DIFFUSE = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_DIFFUSE);
    programs.CS_SAMPLER_MAPPING_SPECULAR = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_SPECULAR);
    programs.CS_SAMPLER_MAPPING_NORMAL = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_NORMAL);
    programs.CS_SAMPLER_MAPPING_REFLECTIONSTRENGTH = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_REFLECTIONSTRENGTH);
    programs.CS_SAMPLER_MAPPING_JITTER = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_JITTER);
    programs.CS_COLOR_MAPPING_AMBIENT = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_AMBIENT);
    programs.CS_COLOR_MAPPING_DIFFUSE = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_DIFFUSE);
    programs.CS_COLOR_MAPPING_SPECULAR = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_SPECULAR);
    programs.CS_SWITCH_TEXTURE_MAPPING = glGetUniformLocation(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SWITCH_TEXTURE_MAPPING);

    // SS vertex shader
    programs.SS_MAT_MODEL = glGetUniformLocation(programs.PROGRAM_ID_SS, ALGINE_NAME_SS_MAT_MODEL);
    programs.SS_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_SS, ALGINE_NAME_SS_IN_POSITION);
    
    // SS geometry shader
    // ALGINE_SS_MAT_SHADOW[6],

    // SS fragment shader
    programs.SS_LAMP_POSITION = glGetUniformLocation(programs.PROGRAM_ID_SS, ALGINE_NAME_SS_LAMP_POSITION);
    programs.SS_FAR_PLANE = glGetUniformLocation(programs.PROGRAM_ID_SS, ALGINE_NAME_SS_FAR_PLANE);

    // SSS vertex shader
    programs.SSS_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_IN_POSITION);
    programs.SSS_IN_TEXCOORD = glGetAttribLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_IN_TEXCOORD);

    // SSS fragment shader
    programs.SSS_SAMPLER_MAP_COLOR = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SAMPLER_MAP_COLOR);
    programs.SSS_SAMPLER_MAP_NORMAL = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SAMPLER_MAP_NORMAL);
    programs.SSS_SAMPLER_MAP_SSRVALUES = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SAMPLER_MAP_SSRVALUES);
    programs.SSS_SAMPLER_MAP_POSITION = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SAMPLER_MAP_POSITION);
    programs.SSS_MAT_PROJECTION = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_MAT_PROJECTION);
    programs.SSS_MAT_VIEW = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_MAT_VIEW);
    programs.SSS_SSR_SKYCOLOR = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_SKYCOLOR);
    programs.SSS_SSR_BINARYSEARCHCOUNT = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_BINARYSEARCHCOUNT);
    programs.SSS_SSR_RAYMARCHCOUNT = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_RAYMARCHCOUNT);
    programs.SSS_SSR_STEP = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_STEP);
    programs.SSS_SSR_LLIMITER = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_LLIMITER);
    programs.SSS_SSR_MINRAYSTEP = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_SSR_MINRAYSTEP);
    programs.SSS_BL_VECTHRESHOLD = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_BL_VECTHRESHOLD);
    programs.SSS_BL_BRIGHTNESSTHRESHOLD = glGetUniformLocation(programs.PROGRAM_ID_SSS, ALGINE_NAME_SSS_BL_BRIGHTNESSTHRESHOLD);

    // BLUS_HORIZONTAL vertex shader
    programs.BLUS_H_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_IN_POSITION);
    programs.BLUS_H_IN_TEXCOORD = glGetAttribLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_IN_TEXCOORD);
    
    // BLUS_HORIZONTAL fragment shader
    programs.BLUS_H_SAMPLER_BLOOM = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SAMPLER_BLOOM);
    programs.BLUS_H_SAMPLER_SCENE = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SAMPLER_SCENE);
    programs.BLUS_H_SAMPLER_DOF_BUFFER = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SAMPLER_DOF_BUFFER);
    programs.BLUS_H_SIGMA_MIN = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SIGMA_MIN);
    programs.BLUS_H_SIGMA_MAX = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SIGMA_MAX);
    programs.BLUS_H_KERNEL_BLOOM = new GLuint[params.KERNEL_BLOOM_SIZE];
    for (size_t i = 0; i < params.KERNEL_BLOOM_SIZE; i++) programs.BLUS_H_KERNEL_BLOOM[i] = glGetUniformLocation(programs.PROGRAM_ID_BLUS_HORIZONTAL, (std::string(ALGINE_NAME_BLUS_KERNEL_BLOOM) + "[" + std::to_string(i) + "]").c_str());

    // BLUS_VERTICAL vertex shader
    programs.BLUS_V_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_IN_POSITION);
    programs.BLUS_V_IN_TEXCOORD = glGetAttribLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_IN_TEXCOORD);
    
    // BLUS_VERTICAL fragment shader
    programs.BLUS_V_SAMPLER_BLOOM = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SAMPLER_BLOOM);
    programs.BLUS_V_SAMPLER_SCENE = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SAMPLER_SCENE);
    programs.BLUS_V_SAMPLER_DOF_BUFFER = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SAMPLER_DOF_BUFFER);
    programs.BLUS_V_SIGMA_MIN = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SIGMA_MIN);
    programs.BLUS_V_SIGMA_MAX = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SIGMA_MAX);
    programs.BLUS_V_KERNEL_BLOOM = new GLuint[params.KERNEL_BLOOM_SIZE];
    for (size_t i = 0; i < params.KERNEL_BLOOM_SIZE; i++) programs.BLUS_V_KERNEL_BLOOM[i] = glGetUniformLocation(programs.PROGRAM_ID_BLUS_VERTICAL, (std::string(ALGINE_NAME_BLUS_KERNEL_BLOOM) + "[" + std::to_string(i) + "]").c_str());

    // BLES vertex shader
    programs.BLES_IN_POSITION = glGetAttribLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_IN_POSITION);
    programs.BLES_IN_TEXCOORD = glGetAttribLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_IN_TEXCOORD);

    // BLES fragment shader
    programs.BLES_SAMPLER_BLOOM_SCENE = glGetUniformLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_SAMPLER_BLOOM_SCENE);
    programs.BLES_SAMPLER_DOF_SCENE = glGetUniformLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_SAMPLER_DOF_SCENE);
    programs.BLES_EXPOSURE = glGetUniformLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_EXPOSURE);
    programs.BLES_GAMMA = glGetUniformLocation(programs.PROGRAM_ID_BLES, ALGINE_NAME_BLES_GAMMA);
}

} // namespace scompiler
} // namespace algine
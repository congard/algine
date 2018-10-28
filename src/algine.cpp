#include <iostream>

#include <GL/glew.h>

#include "utils/shader_utils.cpp"
#include "utils/math.cpp"

/**
 * Algine constants
 */

#define ALGINE_NORMAL_MAPPING_MODE_ENABLED 0
#define ALGINE_NORMAL_MAPPING_MODE_DUAL 1
#define ALGINE_NORMAL_MAPPING_MODE_DISABLED 2

#define ALGINE_SHADOW_MAPPING_MODE_ENABLED 3
#define ALGINE_SHADOW_MAPPING_MODE_SIMPLE 4
#define ALGINE_SHADOW_MAPPING_MODE_DISABLED 5

#define ALGINE_BLOOM_MODE_ENABLED 6
#define ALGINE_BLOOM_MODE_DISABLED 7

#define ALGINE_DOF_MODE_ENABLED 8
#define ALGINE_DOF_MODE_DISABLED 9

#define ALGINE_TEXTURE_MAPPING_MODE_ENABLED 10
#define ALGINE_TEXTURE_MAPPING_MODE_DUAL 11
#define ALGINE_TEXTURE_MAPPING_MODE_DISABLED 12

#define ALGINE_LIGHTING_MODE_ENABLED 13
#define ALGINE_LIGHTING_MODE_DISABLED 14

#define ALGINE_ATTENUATION_MODE_ENABLED 15
#define ALGINE_ATTENUATION_MODE_DISABLED 16

// CS - color shader
GLuint
    // vertex shader
    ALGINE_CS_MAT_MODEL,
    ALGINE_CS_MAT_VIEW,
    ALGINE_CS_MAT_PVM,
    ALGINE_CS_MAT_VM,
    ALGINE_CS_SWITCH_NORMAL_MAPPING,
    ALGINE_CS_IN_POSITION,
    ALGINE_CS_IN_NORMAL,
    ALGINE_CS_IN_TANGENT,
    ALGINE_CS_IN_BITANGENT,
    ALGINE_CS_IN_TEXCOORD,
    
    // fragment shader
    ALGINE_CS_VIEW_POSITION,
    ALGINE_CS_LAMPS_COUNT,
    ALGINE_CS_BRIGHTNESS_THRESHOLD,
    ALGINE_CS_SHADOW_FAR_PLANE,
    ALGINE_CS_SHADOW_DISKRADIUS_K,
    ALGINE_CS_SHADOW_DISKRADIUS_MIN,
    ALGINE_CS_SHADOW_BIAS,
    ALGINE_CS_FOCAL_DEPTH,
    ALGINE_CS_FOCAL_RANGE,
    //*ALGINE_CS_SHADOW_MAPS, // The loading of these locations is performed by the class TextureArray (utils/texture_utils.cpp)
    ALGINE_CS_SAMPLER_MAPPING_AMBIENT,
    ALGINE_CS_SAMPLER_MAPPING_DIFFUSE,
    ALGINE_CS_SAMPLER_MAPPING_SPECULAR,
    ALGINE_CS_SAMPLER_MAPPING_NORMAL,
    ALGINE_CS_COLOR_MAPPING_AMBIENT,
    ALGINE_CS_COLOR_MAPPING_DIFFUSE,
    ALGINE_CS_COLOR_MAPPING_SPECULAR,
    ALGINE_CS_SWITCH_TEXTURE_MAPPING;
    // *ALGINE_CS_LAMPS; // The loading of these locations is performed by the class Lamp (lamp.cpp)

// SS - shadow shader
GLuint
    // vertex shader
    ALGINE_SS_MAT_MODEL,
    ALGINE_SS_IN_POSITION,
    
    // geometry shader
    // ALGINE_SS_MAT_SHADOW[6], // The loading of these locations is performed by the class Lamp (lamp.cpp)

    // fragment shader
    ALGINE_SS_LAMP_POSITION,
    ALGINE_SS_FAR_PLANE;

// BLUS_HORIZONTAL - blur horizontal shader
GLuint
    // vertex shader
    ALGINE_BLUS_H_IN_POSITION,
    ALGINE_BLUS_H_IN_TEXCOORD,
    
    // fragment shader
    ALGINE_BLUS_H_SAMPLER_BLOOM,
    ALGINE_BLUS_H_SAMPLER_DOF,
    ALGINE_BLUS_H_SIGMA_MIN,
    ALGINE_BLUS_H_SIGMA_MAX,
   *ALGINE_BLUS_H_KERNEL_BLOOM;

// BLUS_VERTICAL - blur horizontal shader
GLuint
    // vertex shader
    ALGINE_BLUS_V_IN_POSITION,
    ALGINE_BLUS_V_IN_TEXCOORD,
    
    // fragment shader
    ALGINE_BLUS_V_SAMPLER_BLOOM,
    ALGINE_BLUS_V_SAMPLER_DOF,
    ALGINE_BLUS_V_SIGMA_MIN,
    ALGINE_BLUS_V_SIGMA_MAX,
   *ALGINE_BLUS_V_KERNEL_BLOOM;

// BLES - blend shader
GLuint
    // vertex shader
    ALGINE_BLES_IN_POSITION,
    ALGINE_BLES_IN_TEXCOORD,

    // fragment shader
    ALGINE_BLES_SAMPLER_BLOOM_SCENE,
    ALGINE_BLES_SAMPLER_DOF_SCENE,
    ALGINE_BLES_EXPOSURE,
    ALGINE_BLES_GAMMA;

// switches
// if ALGINE_LIGHTING_MODE == ALGINE_LIGHTING_MODE_DISABLED then it turns off and ALGINE_SHADOW_MAPPING_MODE and ALGINE_ATTENUATION_MODE 
GLubyte
    ALGINE_NORMAL_MAPPING_MODE = ALGINE_NORMAL_MAPPING_MODE_ENABLED,
    ALGINE_SHADOW_MAPPING_MODE = ALGINE_SHADOW_MAPPING_MODE_ENABLED,
    ALGINE_BLOOM_MODE = ALGINE_BLOOM_MODE_ENABLED,
    ALGINE_DOF_MODE = ALGINE_DOF_MODE_ENABLED,
    ALGINE_TEXTURE_MAPPING_MODE = ALGINE_TEXTURE_MAPPING_MODE_ENABLED,
    ALGINE_LIGHTING_MODE = ALGINE_LIGHTING_MODE_ENABLED,
    ALGINE_ATTENUATION_MODE = ALGINE_ATTENUATION_MODE_ENABLED;

GLuint
    ALGINE_SCR_W = 1366,
    ALGINE_SCR_H = 768,

    ALGINE_MAX_LAMPS_COUNT = 8,
    ALGINE_KERNEL_BLOOM_SIZE = 8,
    ALGINE_KERNEL_DOF_SIZE = 8,
    
    ALGINE_BLUR_AMOUNT = 4;

GLfloat
    ALGINE_GAMMA = 1.0f,
    ALGINE_EXPOSURE = 3.0f,
    ALGINE_KERNEL_BLOOM_SIGMA = 8.0f;

GLuint
    ALGINE_PROGRAM_ID_CS,
    ALGINE_PROGRAM_ID_SS,
    ALGINE_PROGRAM_ID_BLUS_HORIZONTAL,
    ALGINE_PROGRAM_ID_BLUS_VERTICAL,
    ALGINE_PROGRAM_ID_BLES;

namespace algine_private_tools_namespace {
    void split(std::vector<std::string> &out, std::string &in, const std::string &delimiter) {
        size_t pos = 0;
        std::string token;
        while ((pos = in.find(delimiter)) != std::string::npos) {
            token = in.substr(0, pos);
            out.push_back(token);
            in.erase(0, pos + delimiter.length());
        }
        out.push_back(in);
    }
}

struct ALGINE_PATHS_TO_SHADERS {
    const char
        *ALGINE_CS_PATH_TO_SHADER_SOURCE_VERTEX,
        *ALGINE_CS_PATH_TO_SHADER_SOURCE_FRAGMENT,

        *ALGINE_SS_PATH_TO_SHADER_SOURCE_VERTEX,
        *ALGINE_SS_PATH_TO_SHADER_SOURCE_GEOMETRY,
        *ALGINE_SS_PATH_TO_SHADER_SOURCE_FRAGMENT,
    
        *ALGINE_BLUS_PATH_TO_SHADER_SOURCE_VERTEX,
        *ALGINE_BLUS_PATH_TO_SHADER_SOURCE_FRAGMENT,

        *ALGINE_BLES_PATH_TO_SHADER_SOURCE_VERTEX,
        *ALGINE_BLES_PATH_TO_SHADER_SOURCE_FRAGMENT;
};

struct ALGINE_PROCESSED_SHADERS {
    std::string
        ALGINE_CS_SHADER_PROCESSED_SOURCE_VERTEX,
        ALGINE_CS_SHADER_PROCESSED_SOURCE_FRAGMENT,

        ALGINE_SS_SHADER_PROCESSED_SOURCE_VERTEX,
        ALGINE_SS_SHADER_PROCESSED_SOURCE_GEOMETRY,
        ALGINE_SS_SHADER_PROCESSED_SOURCE_FRAGMENT,
        
        ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX,
        ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL,
        ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_VERTICAL,

        ALGINE_BLES_SHADER_PROCESSED_SOURCE_VERTEX,
        ALGINE_BLES_SHADER_PROCESSED_SOURCE_FRAGMENT;
};

#define apn algine_private_tools_namespace
#define ALGINE_DEF_DELIMITER std::string("#algdef")

ALGINE_PROCESSED_SHADERS algine_create_shaders_sources(ALGINE_PATHS_TO_SHADERS &apts) {
    // loading original shaders
    char
        *ALGINE_CS_SHADER_SOURCE_VERTEX = readShader(apts.ALGINE_CS_PATH_TO_SHADER_SOURCE_VERTEX),
        *ALGINE_CS_SHADER_SOURCE_FRAGMENT = readShader(apts.ALGINE_CS_PATH_TO_SHADER_SOURCE_FRAGMENT),

        *ALGINE_SS_SHADER_SOURCE_VERTEX = readShader(apts.ALGINE_SS_PATH_TO_SHADER_SOURCE_VERTEX),
        *ALGINE_SS_SHADER_SOURCE_GEOMETRY = readShader(apts.ALGINE_SS_PATH_TO_SHADER_SOURCE_GEOMETRY),
        *ALGINE_SS_SHADER_SOURCE_FRAGMENT = readShader(apts.ALGINE_SS_PATH_TO_SHADER_SOURCE_FRAGMENT),
        
        *ALGINE_BLUS_SHADER_SOURCE_VERTEX = readShader(apts.ALGINE_BLUS_PATH_TO_SHADER_SOURCE_VERTEX),
        *ALGINE_BLUS_SHADER_SOURCE_FRAGMENT = readShader(apts.ALGINE_BLUS_PATH_TO_SHADER_SOURCE_FRAGMENT),

        *ALGINE_BLES_SHADER_SOURCE_VERTEX = readShader(apts.ALGINE_BLES_PATH_TO_SHADER_SOURCE_VERTEX),
        *ALGINE_BLES_SHADER_SOURCE_FRAGMENT = readShader(apts.ALGINE_BLES_PATH_TO_SHADER_SOURCE_FRAGMENT);

    ALGINE_PROCESSED_SHADERS aps;

    std::vector<std::string> out;
    std::string in;

    // CS shaders
    // vertex shader
    apn::split(out, in = ALGINE_CS_SHADER_SOURCE_VERTEX, ALGINE_DEF_DELIMITER);
    aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_VERTEX = 
        out[0] + (
            ALGINE_NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            ALGINE_NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + out[1];

    // fragment shader
    out.clear();
    apn::split(out, in = ALGINE_CS_SHADER_SOURCE_FRAGMENT, ALGINE_DEF_DELIMITER);
    aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_FRAGMENT = 
        out[0] + (
            ALGINE_NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_ENABLED ? "#define ALGINE_NORMAL_MAPPING_MODE_ENABLED\n" :
            ALGINE_NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_DUAL ? "#define ALGINE_NORMAL_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_NORMAL_MAPPING_MODE_DISABLED\n"
        ) + (
            ALGINE_SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_ENABLED ? "#define ALGINE_SHADOW_MAPPING_MODE_ENABLED\n" :
            ALGINE_SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_SIMPLE ? "#define ALGINE_SHADOW_MAPPING_MODE_SIMPLE\n" : 
            "#define ALGINE_SHADOW_MAPPING_MODE_DISABLED\n"
        ) + (
            ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" :
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" :
            "#define ALGINE_DOF_MODE_DISABLED\n"
        ) + (
            ALGINE_TEXTURE_MAPPING_MODE == ALGINE_TEXTURE_MAPPING_MODE_ENABLED ? "#define ALGINE_TEXTURE_MAPPING_MODE_ENABLED\n" :
            ALGINE_TEXTURE_MAPPING_MODE == ALGINE_TEXTURE_MAPPING_MODE_DUAL ? "#define ALGINE_TEXTURE_MAPPING_MODE_DUAL\n" : 
            "#define ALGINE_TEXTURE_MAPPING_MODE_DISABLED\n"
        ) + (
            ALGINE_LIGHTING_MODE == ALGINE_LIGHTING_MODE_ENABLED ? "#define ALGINE_LIGHTING_MODE_ENABLED\n" :
            "#define ALGINE_LIGHTING_MODE_DISABLED\n"
        ) + (
            ALGINE_ATTENUATION_MODE == ALGINE_ATTENUATION_MODE_ENABLED ? "#define ALGINE_ATTENUATION_MODE_ENABLED\n" :
            "#define ALGINE_ATTENUATION_MODE_DISABLED\n"
        ) + (
            "#define MAX_LAMPS_COUNT " + std::to_string(ALGINE_MAX_LAMPS_COUNT) + "\n"
        ) + out[1];

    // SS shaders
    if (ALGINE_SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_ENABLED || ALGINE_SHADOW_MAPPING_MODE == ALGINE_SHADOW_MAPPING_MODE_SIMPLE) {
        aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_VERTEX = ALGINE_SS_SHADER_SOURCE_VERTEX;
        aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_GEOMETRY = ALGINE_SS_SHADER_SOURCE_GEOMETRY;
        aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_FRAGMENT = ALGINE_SS_SHADER_SOURCE_FRAGMENT;
    }

    // BLUS shaders
    if (ALGINE_BLOOM_MODE != ALGINE_BLOOM_MODE_DISABLED || ALGINE_DOF_MODE != ALGINE_DOF_MODE_DISABLED) {
        // vertex shader
        aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX = ALGINE_BLUS_SHADER_SOURCE_VERTEX;
        // fragment shader horizontal 1st part
        out.clear();
        apn::split(out, in = ALGINE_BLUS_SHADER_SOURCE_FRAGMENT, ALGINE_DEF_DELIMITER);
        aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL = 
            out[0] + (
                ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n#define BLOOM_KERNEL_SIZE " + std::to_string(ALGINE_KERNEL_BLOOM_SIZE) + "\n" : 
                "#define ALGINE_BLOOM_MODE_DISABLED\n"
            ) + (
                ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n#define DOF_KERNEL_SIZE " + std::to_string(ALGINE_KERNEL_DOF_SIZE) + "\n" : 
                "#define ALGINE_DOF_MODE_DISABLED\n"
            );
        // fragment shader vertical 1st and 2nd part
        aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_VERTICAL = aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL + out[1];
        // fragment shader horizontal 2nd part
        aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL += "#define ALGINE_BLUS_HORIZONTAL\n" + out[1];
    }

    // BLES shaders
    // vertex shader
    aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_VERTEX = ALGINE_BLES_SHADER_SOURCE_VERTEX;
    // fragment shader
    out.clear();
    apn::split(out, in = ALGINE_BLES_SHADER_SOURCE_FRAGMENT, ALGINE_DEF_DELIMITER);
    aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_FRAGMENT = 
        out[0] + (
            ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? "#define ALGINE_BLOOM_MODE_ENABLED\n" : 
            "#define ALGINE_BLOOM_MODE_DISABLED\n"
        ) + (
            ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED ? "#define ALGINE_DOF_MODE_ENABLED\n" : 
            "#define ALGINE_DOF_MODE_DISABLED\n"
        ) + out[1];

    // free memory
    delete[] ALGINE_CS_SHADER_SOURCE_VERTEX;
    delete[] ALGINE_CS_SHADER_SOURCE_FRAGMENT;

    delete[] ALGINE_SS_SHADER_SOURCE_VERTEX;
    delete[] ALGINE_SS_SHADER_SOURCE_GEOMETRY;
    delete[] ALGINE_SS_SHADER_SOURCE_FRAGMENT;

    delete[] ALGINE_BLUS_SHADER_SOURCE_VERTEX;
    delete[] ALGINE_BLUS_SHADER_SOURCE_FRAGMENT;

    delete[] ALGINE_BLES_SHADER_SOURCE_VERTEX;
    delete[] ALGINE_BLES_SHADER_SOURCE_FRAGMENT;

    return aps;
}

#undef apn

void algine_save_shaders(const ALGINE_PROCESSED_SHADERS &aps, const std::string &path) {
    const std::string *sources[10] = {
        &aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_VERTEX,
        &aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_FRAGMENT,

        &aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_VERTEX,
        &aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_GEOMETRY,
        &aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_FRAGMENT,
        
        &aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX,
        &aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL,
        &aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_VERTICAL,

        &aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_VERTEX,
        &aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_FRAGMENT
    };

    const char *names[10] = {
        "/ALGINE_CS_SHADER_PROCESSED_SOURCE_VERTEX.glsl",
        "/ALGINE_CS_SHADER_PROCESSED_SOURCE_FRAGMENT.glsl",

        "/ALGINE_SS_SHADER_PROCESSED_SOURCE_VERTEX.glsl",
        "/ALGINE_SS_SHADER_PROCESSED_SOURCE_GEOMETRY.glsl",
        "/ALGINE_SS_SHADER_PROCESSED_SOURCE_FRAGMENT.glsl",
        
        "/ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX.glsl",
        "/ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL.glsl",
        "/ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_VERTICAL.glsl",

        "/ALGINE_BLES_SHADER_PROCESSED_SOURCE_VERTEX.glsl",
        "/ALGINE_BLES_SHADER_PROCESSED_SOURCE_FRAGMENT.glsl"
    };

    FILE *file;

    for (GLuint i = 0; i < 10; i++) {
        fwrite(
            sources[i]->c_str(),
            1, sources[i]->size(),
            file = fopen((path + names[i]).c_str(), "wb")
        );
        fclose(file);
    }
}

#undef ALGINE_DEF_DELIMITER

#include "sconstants.h"

/* --- compiling shaders --- */
void algine_compile_shaders(const ALGINE_PROCESSED_SHADERS &aps) {
    mksp_source(ALGINE_PROGRAM_ID_CS, aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_VERTEX.c_str(), aps.ALGINE_CS_SHADER_PROCESSED_SOURCE_FRAGMENT.c_str(), nullptr);
    mksp_source(ALGINE_PROGRAM_ID_SS, aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_VERTEX.c_str(), aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_FRAGMENT.c_str(), aps.ALGINE_SS_SHADER_PROCESSED_SOURCE_GEOMETRY.c_str());
    mksp_source(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX.c_str(), aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_HORIZONTAL.c_str(), nullptr);
    mksp_source(ALGINE_PROGRAM_ID_BLUS_VERTICAL, aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_VERTEX.c_str(), aps.ALGINE_BLUS_SHADER_PROCESSED_SOURCE_FRAGMENT_VERTICAL.c_str(), nullptr);
    mksp_source(ALGINE_PROGRAM_ID_BLES, aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_VERTEX.c_str(), aps.ALGINE_BLES_SHADER_PROCESSED_SOURCE_FRAGMENT.c_str(), nullptr);
}

/* --- loading shaders variables locations --- */
void algine_load_locations() {
    // CS vertex shader
    ALGINE_CS_MAT_MODEL = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_MODEL);
    ALGINE_CS_MAT_VIEW = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_VIEW);
    ALGINE_CS_MAT_PVM = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_PVM);
    ALGINE_CS_MAT_VM = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_MAT_VM);
    ALGINE_CS_SWITCH_NORMAL_MAPPING = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SWITCH_NORMAL_MAPPING);
    ALGINE_CS_IN_POSITION = glGetAttribLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_IN_POSITION);
    ALGINE_CS_IN_NORMAL = glGetAttribLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_IN_NORMAL);
    ALGINE_CS_IN_TANGENT = glGetAttribLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_IN_TANGENT);
    ALGINE_CS_IN_BITANGENT = glGetAttribLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_IN_BITANGENT);
    ALGINE_CS_IN_TEXCOORD = glGetAttribLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_IN_TEXCOORD);
    
    // CS fragment shader
    ALGINE_CS_VIEW_POSITION = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_VIEW_POSITION);
    ALGINE_CS_LAMPS_COUNT = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_LAMPS_COUNT);
    ALGINE_CS_BRIGHTNESS_THRESHOLD = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_BRIGHTNESS_THRESHOLD);
    ALGINE_CS_SHADOW_FAR_PLANE = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_FAR_PLANE);
    ALGINE_CS_SHADOW_DISKRADIUS_K = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_DISKRADIUS_K);
    ALGINE_CS_SHADOW_DISKRADIUS_MIN = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_DISKRADIUS_MIN);
    ALGINE_CS_SHADOW_BIAS = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_BIAS);
    ALGINE_CS_FOCAL_DEPTH = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_FOCAL_DEPTH);
    ALGINE_CS_FOCAL_RANGE = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_FOCAL_RANGE);
    ALGINE_CS_SAMPLER_MAPPING_AMBIENT = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_AMBIENT);
    ALGINE_CS_SAMPLER_MAPPING_DIFFUSE = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_DIFFUSE);
    ALGINE_CS_SAMPLER_MAPPING_SPECULAR = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_SPECULAR);
    ALGINE_CS_SAMPLER_MAPPING_NORMAL = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SAMPLER_MAPPING_NORMAL);
    ALGINE_CS_COLOR_MAPPING_AMBIENT = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_AMBIENT);
    ALGINE_CS_COLOR_MAPPING_DIFFUSE = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_DIFFUSE);
    ALGINE_CS_COLOR_MAPPING_SPECULAR = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_COLOR_MAPPING_SPECULAR);
    ALGINE_CS_SWITCH_TEXTURE_MAPPING = glGetUniformLocation(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SWITCH_TEXTURE_MAPPING);

    // SS vertex shader
    ALGINE_SS_MAT_MODEL = glGetUniformLocation(ALGINE_PROGRAM_ID_SS, ALGINE_NAME_SS_MAT_MODEL);
    ALGINE_SS_IN_POSITION = glGetAttribLocation(ALGINE_PROGRAM_ID_SS, ALGINE_NAME_SS_IN_POSITION);
    
    // SS geometry shader
    // ALGINE_SS_MAT_SHADOW[6],

    // SS fragment shader
    ALGINE_SS_LAMP_POSITION = glGetUniformLocation(ALGINE_PROGRAM_ID_SS, ALGINE_NAME_SS_LAMP_POSITION);
    ALGINE_SS_FAR_PLANE = glGetUniformLocation(ALGINE_PROGRAM_ID_SS, ALGINE_NAME_SS_FAR_PLANE);

    // BLUS_HORIZONTAL vertex shader
    ALGINE_BLUS_H_IN_POSITION = glGetAttribLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_IN_POSITION);
    ALGINE_BLUS_H_IN_TEXCOORD = glGetAttribLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_IN_TEXCOORD);
    
    // BLUS_HORIZONTAL fragment shader
    ALGINE_BLUS_H_SAMPLER_BLOOM = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SAMPLER_BLOOM);
    ALGINE_BLUS_H_SAMPLER_DOF = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SAMPLER_DOF);
    ALGINE_BLUS_H_SIGMA_MIN = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SIGMA_MIN);
    ALGINE_BLUS_H_SIGMA_MAX = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_NAME_BLUS_SIGMA_MAX);
    ALGINE_BLUS_H_KERNEL_BLOOM = new GLuint[ALGINE_KERNEL_BLOOM_SIZE];
    for (size_t i = 0; i < ALGINE_KERNEL_BLOOM_SIZE; i++) ALGINE_BLUS_H_KERNEL_BLOOM[i] = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, (std::string(ALGINE_NAME_BLUS_KERNEL_BLOOM) + "[" + std::to_string(i) + "]").c_str());

    // BLUS_VERTICAL vertex shader
    ALGINE_BLUS_V_IN_POSITION = glGetAttribLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_IN_POSITION);
    ALGINE_BLUS_V_IN_TEXCOORD = glGetAttribLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_IN_TEXCOORD);
    
    // BLUS_VERTICAL fragment shader
    ALGINE_BLUS_V_SAMPLER_BLOOM = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SAMPLER_BLOOM);
    ALGINE_BLUS_V_SAMPLER_DOF = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SAMPLER_DOF);
    ALGINE_BLUS_V_SIGMA_MIN = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SIGMA_MIN);
    ALGINE_BLUS_V_SIGMA_MAX = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_NAME_BLUS_SIGMA_MAX);
    ALGINE_BLUS_V_KERNEL_BLOOM = new GLuint[ALGINE_KERNEL_BLOOM_SIZE];
    for (size_t i = 0; i < ALGINE_KERNEL_BLOOM_SIZE; i++) ALGINE_BLUS_V_KERNEL_BLOOM[i] = glGetUniformLocation(ALGINE_PROGRAM_ID_BLUS_VERTICAL, (std::string(ALGINE_NAME_BLUS_KERNEL_BLOOM) + "[" + std::to_string(i) + "]").c_str());

    // BLES vertex shader
    ALGINE_BLES_IN_POSITION = glGetAttribLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_IN_POSITION);
    ALGINE_BLES_IN_TEXCOORD = glGetAttribLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_IN_TEXCOORD);

    // BLES fragment shader
    ALGINE_BLES_SAMPLER_BLOOM_SCENE = glGetUniformLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_SAMPLER_BLOOM_SCENE);
    ALGINE_BLES_SAMPLER_DOF_SCENE = glGetUniformLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_SAMPLER_DOF_SCENE);
    ALGINE_BLES_EXPOSURE = glGetUniformLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_EXPOSURE);
    ALGINE_BLES_GAMMA = glGetUniformLocation(ALGINE_PROGRAM_ID_BLES, ALGINE_NAME_BLES_GAMMA);
}

#include "unsconstants.h"

/* renderer code */
struct AlgineRenderer {
    GLuint displayFBO, 
		pingpongFBO[2], colorBuffers[2], // main, bloom
		pingpongBuffers[2], // blur buffers
		dofBuffers[2], // dof buffers
        rboBuffer, 
        quadBuffers[2], // vertexBuffer and texCoordsBuffer
        colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 },
        colorAttachmentsNONE1[2] = { GL_NONE, GL_COLOR_ATTACHMENT1 };

    struct BlusProgram {
        GLuint
            programId,
            inPos,
            inTexCoord,
            samplerDof;
    };

    BlusProgram blusPrograms[2] = { 
        BlusProgram {
            ALGINE_PROGRAM_ID_BLUS_VERTICAL, ALGINE_BLUS_V_IN_POSITION, ALGINE_BLUS_V_IN_TEXCOORD, ALGINE_BLUS_V_SAMPLER_DOF
        },

        BlusProgram { 
            ALGINE_PROGRAM_ID_BLUS_HORIZONTAL, ALGINE_BLUS_H_IN_POSITION, ALGINE_BLUS_H_IN_TEXCOORD, ALGINE_BLUS_H_SAMPLER_DOF
        }
    };

    bool horizontal = true, firstIteration = true; // blur variables

    /* --- pointers to functions start --- */
    void (AlgineRenderer::*mkBlur)();

    void (AlgineRenderer::*blend)();

    void (AlgineRenderer::*begin_rendering)();

    void (AlgineRenderer::*end_rendering)();
    /* --- pointers to functions end --- */

	void applyDefaultTextureParams() {
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

	void renderQuad(const GLuint &inPosLocation, const GLuint &inTexCoordLocation) {
		glEnableVertexAttribArray(inPosLocation);
        glEnableVertexAttribArray(inTexCoordLocation);
	    su::pointer(inPosLocation, 3, quadBuffers[0]);
		su::pointer(inTexCoordLocation, 2, quadBuffers[1]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
		glDisableVertexAttribArray(inPosLocation);
        glDisableVertexAttribArray(inTexCoordLocation);
	}

    /* --- only bloom --- */
    void mkBlur_bloom() {
        // configuring textures
        glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < 2; i++) {
            // bloom
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGB, GL_FLOAT, NULL);
		}
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < ALGINE_BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            // bloom
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    /* --- only dof --- */
    void mkBlur_dof() {
        // configuring textures
        glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < 2; i++) {
            // dof
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
			glClearColor(0, 0, 0, 1); // for effective dof
		}
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < ALGINE_BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            // dof
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[0] : dofBuffers[!horizontal]);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void mkBlur_bloom_dof() {
        // configuring textures
		for (int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

            // bloom
			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGB, GL_FLOAT, NULL);

            // dof
			glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // far plane (background)
		}
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < ALGINE_BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            // bloom
            glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);

            // dof
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(blusPrograms[horizontal].samplerDof, 1);
			glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[0] : dofBuffers[!horizontal]);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    /* --- blend functions start --- */
    void blend_bloom_dof() {
		// Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(ALGINE_PROGRAM_ID_BLES);
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dofBuffers[!horizontal]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
		renderQuad(ALGINE_BLES_IN_POSITION, ALGINE_BLES_IN_TEXCOORD);
    }

    void blend_bloom() {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(ALGINE_PROGRAM_ID_BLES);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
		renderQuad(ALGINE_BLES_IN_POSITION, ALGINE_BLES_IN_TEXCOORD);
    }

    void blend_dof() {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(ALGINE_PROGRAM_ID_BLES);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dofBuffers[!horizontal]);
		renderQuad(ALGINE_BLES_IN_POSITION, ALGINE_BLES_IN_TEXCOORD);
    }

    void blend_() {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(ALGINE_PROGRAM_ID_BLES);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		renderQuad(ALGINE_BLES_IN_POSITION, ALGINE_BLES_IN_TEXCOORD);
    }
    /* --- blend functions end --- */

    void begin_rendering_bloom() {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, rboBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ALGINE_SCR_W, ALGINE_SCR_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear it

        // configuring textures <screen, bloom>
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
        }
    }

    void begin_rendering_() {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, rboBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ALGINE_SCR_W, ALGINE_SCR_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear it

        // configuring textures <screen>
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    void end_rendering_blur() {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        (this->*mkBlur)();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        (this->*blend)();
        glUseProgram(0);
    }

    void end_rendering_() {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        (this->*blend)();
        glUseProgram(0);
    }

    void prepare() {
        /* --- pointers to functions --- */
        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED && ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED) {
            mkBlur = &AlgineRenderer::mkBlur_bloom_dof;
            blend = &AlgineRenderer::blend_bloom_dof;
        } else if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) {
            mkBlur = &AlgineRenderer::mkBlur_bloom;
            blend = &AlgineRenderer::blend_bloom;
        } else if (ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED) {
            mkBlur = &AlgineRenderer::mkBlur_dof;
            blend = &AlgineRenderer::blend_dof;
        } else {
            blend = &AlgineRenderer::blend_;
        }

        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) {
            begin_rendering = &AlgineRenderer::begin_rendering_bloom;
        } else {
            begin_rendering = &AlgineRenderer::begin_rendering_;
        }

        if (ALGINE_BLOOM_MODE != ALGINE_BLOOM_MODE_ENABLED && ALGINE_DOF_MODE != ALGINE_DOF_MODE_ENABLED)
            end_rendering = &AlgineRenderer::end_rendering_;
        else
            end_rendering = &AlgineRenderer::end_rendering_blur;

        glGenFramebuffers(1, &displayFBO);
        glGenTextures(ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED ? 2 : 1, colorBuffers);
        glGenRenderbuffers(1, &rboBuffer);
        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED || ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED) glGenFramebuffers(2, pingpongFBO);
        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) glGenTextures(2, pingpongBuffers);
        if (ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED) glGenTextures(2, dofBuffers);

        // configuring display framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) glDrawBuffers(2, colorAttachments); // use 2 color components (scene / bloom)
        else glDrawBuffers(1, &colorAttachments[0]); // use 1 color component (scene only)

        // configuring textures
        for (size_t i = 0; i < 2; i++) {
            if ((ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_DISABLED && i == 0) || ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) {
                // configuring render textures
                glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
                glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
                applyDefaultTextureParams();
                // attach texture to framebuffer
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
            }

            // configuring ping-pong (blur) textures
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

            if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED && ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED)
                glDrawBuffers(2, colorAttachments); // use 2 color components (boom / dof)
            else if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED)
                glDrawBuffers(1, &colorAttachments[0]); // use 1 color component (bloom)
            else
                glDrawBuffers(2, colorAttachmentsNONE1); // use 1 color component (none / dof)

            if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) {
                glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
                applyDefaultTextureParams();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
            }
            
            if (ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED) {
                glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
                applyDefaultTextureParams();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dofBuffers[i], 0);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // configuring blur (bloom) kernel
        // sending to shader center of kernel and right part
        if (ALGINE_BLOOM_MODE == ALGINE_BLOOM_MODE_ENABLED) {
            float kernel[(ALGINE_KERNEL_BLOOM_SIZE - 1) * 2];
            getGB1DKernel(kernel, (ALGINE_KERNEL_BLOOM_SIZE - 1) * 2, ALGINE_KERNEL_BLOOM_SIGMA);

            for (size_t j = 0; j < 2; j++) {
                glUseProgram(blusPrograms[j].programId);
                for (GLuint i = 0; i < ALGINE_KERNEL_BLOOM_SIZE; i++)
                    glUniform1f(
                        j ? ALGINE_BLUS_H_KERNEL_BLOOM[ALGINE_KERNEL_BLOOM_SIZE - 1 - i] : ALGINE_BLUS_V_KERNEL_BLOOM[ALGINE_KERNEL_BLOOM_SIZE - 1 - i],
                        kernel[i]);
            }

            glUseProgram(0);
        }

        // creating buffers for quad rendering
        static float
            vertices[12] = {
                -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f
            },

            texCoords[8] = {
                0.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 1.0f,
                1.0f, 0.0f
            };

        glGenBuffers(2, quadBuffers);
        glBindBuffer(GL_ARRAY_BUFFER, quadBuffers[0]); // vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, quadBuffers[1]); // texCoords
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

        // blend setting
        glUseProgram(ALGINE_PROGRAM_ID_BLES);
        glUniform1i(ALGINE_BLES_SAMPLER_BLOOM_SCENE, 1); // GL_TEXTURE1
        glUniform1i(ALGINE_BLES_SAMPLER_DOF_SCENE, 0);   // GL_TEXTURE0
        glUniform1f(ALGINE_BLES_EXPOSURE, ALGINE_EXPOSURE);
        glUniform1f(ALGINE_BLES_GAMMA, ALGINE_GAMMA);
        glUseProgram(0);
    }

    ~AlgineRenderer() {
        glDeleteFramebuffers(1, &displayFBO);
        glDeleteTextures(2, colorBuffers);
        glDeleteRenderbuffers(1, &rboBuffer);
        glDeleteFramebuffers(2, pingpongFBO);
        glDeleteTextures(2, pingpongBuffers);
        glDeleteTextures(2, dofBuffers);
        glDeleteBuffers(2, quadBuffers);
        glDeleteProgram(ALGINE_PROGRAM_ID_CS);
        glDeleteProgram(ALGINE_PROGRAM_ID_SS);
        glDeleteProgram(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL);
        glDeleteProgram(ALGINE_PROGRAM_ID_BLUS_VERTICAL);
        glDeleteProgram(ALGINE_PROGRAM_ID_BLES);

        delete[] ALGINE_BLUS_H_KERNEL_BLOOM;
        delete[] ALGINE_BLUS_V_KERNEL_BLOOM;

        #ifdef ALGINE_LOGGING
        std::cout << "~AlgineRenderer() " << this << "\n";
        #endif
    }
};
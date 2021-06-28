#ifndef ALGINE_MODULE_LIGHTING_GLSL
#define ALGINE_MODULE_LIGHTING_GLSL

struct DirLight {
    float kc; // constant term
    float kl; // linear term
    float kq; // quadratic term
    float minBias, maxBias;
    vec3 pos; // in world space
    vec3 color;
    mat4 lightMatrix;
};

struct PointLight {
    float kc; // constant term
    float kl; // linear term
    float kq; // quadratic term
    float far; // shadow matrix far plane
    float bias;
    vec3 pos; // in world space
    vec3 color;
};

layout(packed) uniform Lighting {
    DirLight dirLights[MAX_DIR_LIGHTS_COUNT];
    PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

    uint pointLightsCount;
    uint dirLightsCount;

    float shadowOpacity;
    float diskRadius_k;
    float diskRadius_min;
};

uniform samplerCube pointLightShadowMaps[MAX_POINT_LIGHTS_COUNT];
uniform sampler2D dirLightShadowMaps[MAX_DIR_LIGHTS_COUNT];

#endif //ALGINE_MODULE_LIGHTING_GLSL

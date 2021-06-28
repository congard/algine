#ifndef ALGINE_MODULE_SHADING_DIR_LIGHT_SHADOW
#define ALGINE_MODULE_SHADING_DIR_LIGHT_SHADOW

float dirLightShadow(
    mat4 lightMatrix,
    vec3 fragPos, // world space
    vec3 normal, // the same space as lightDir
    vec3 lightDir, // the same space as normal
    float minBias,
    float maxBias,
    in sampler2D shadowMap
) {
    vec4 fragPosLightSpace = lightMatrix * vec4(fragPos, 1.0f);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // perspective divide
    projCoords = projCoords * 0.5f + 0.5f; // to [0; 1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r; // get closest depth value from light’s perspective (using [0; 1] range projCoords as coords)
    float currentDepth = projCoords.z; // get depth of current fragment from light’s perspective

    float bias = max(maxBias * (1.0f - dot(normal, lightDir)), minBias);

    return currentDepth - bias > closestDepth ? 1.0f : 0.0f;
}

float dirLightSoftShadow(
    mat4 lightMatrix,
    vec3 fragPos, // world space
    vec3 normal, // the same space as lightDir
    vec3 lightDir, // the same space as normal
    float minBias,
    float maxBias,
    in sampler2D shadowMap
) {
    vec4 fragPosLightSpace = lightMatrix * vec4(fragPos, 1.0f);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // perspective divide
    projCoords = projCoords * 0.5f + 0.5f; // to [0; 1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r; // get closest depth value from light’s perspective (using [0; 1] range projCoords as coords)
    float currentDepth = projCoords.z; // get depth of current fragment from light’s perspective

    float bias = max(maxBias * (1.0f - dot(normal, lightDir)), minBias);

    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    float shadow = 0.0f;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
        }
    }

    return shadow /= 9.0f;
}

#endif //ALGINE_MODULE_SHADING_DIR_LIGHT_SHADOW

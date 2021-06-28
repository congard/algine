#ifndef ALGINE_MODULE_SHADING_POINT_LIGHT_SHADOW
#define ALGINE_MODULE_SHADING_POINT_LIGHT_SHADOW

float pointLightShadow(
    vec3 fragPos, // world space
    vec3 lightPos, // world space
    float bias,
    float shadowMapFar,
    in samplerCube shadowMap
) {
    vec3 fragToLight = fragPos - lightPos; // vector between fragment position and light position
    float currentDepth = length(fragToLight); // current linear depth

    float closestDepth = texture(shadowMap, fragToLight).r;
    closestDepth *= shadowMapFar; // Undo mapping [0; 1]

    return currentDepth - bias > closestDepth ? 1.0f : 0.0f;
}

float pointLightSoftShadow(
    vec3 fragPos, // world space
    vec3 lightPos, // world space
    float viewDistance, // distance between camera and fragment
    float diskRadiusK,
    float diskRadiusMin,
    float bias,
    float shadowMapFar,
    in samplerCube shadowMap
) {
    const vec3 sampleOffsetDirections[20] = vec3[] (
        vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
        vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
        vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
        vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
    );

    vec3 fragToLight = fragPos - lightPos; // vector between fragment position and light position
    float currentDepth = length(fragToLight); // current linear depth

    float diskRadius = (1.0f + (viewDistance / shadowMapFar)) * diskRadiusK + diskRadiusMin;

    float shadow = 0.0f;

    for (int i = 0; i < 20; i++) {
        float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= shadowMapFar; // Undo mapping [0; 1]

        if (currentDepth - bias > closestDepth) {
            shadow += 1.0f;
        }
    }

    return shadow /= 20.0f;
}

#endif //ALGINE_MODULE_SHADING_POINT_LIGHT_SHADOW

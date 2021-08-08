#ifndef ALGINE_MODULE_SHADING_DIFFUSE_OREN_NAYAR
#define ALGINE_MODULE_SHADING_DIFFUSE_OREN_NAYAR

// Based on: https://github.com/glslify/glsl-diffuse-oren-nayar
float diffuseOrenNayar(
    vec3 lightDirection,
    vec3 viewDirection,
    vec3 surfaceNormal,
    float roughness,
    float albedo
) {
    const float PI = 3.14159265f;

    float LdotV = dot(lightDirection, viewDirection);
    float NdotL = dot(lightDirection, surfaceNormal);
    float NdotV = dot(surfaceNormal, viewDirection);

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0f, max(NdotL, NdotV), step(0.0f, s));

    float sigma2 = roughness * roughness;
    float A = 1.0f + sigma2 * (albedo / (sigma2 + 0.13f) + 0.5f / (sigma2 + 0.33f));
    float B = 0.45f * sigma2 / (sigma2 + 0.09f);

    return albedo * max(0.0f, NdotL) * (A + B * s / t) / PI;
}

#endif //ALGINE_MODULE_SHADING_DIFFUSE_OREN_NAYAR

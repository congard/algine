#ifndef ALGINE_MODULE_SSR
#define ALGINE_MODULE_SSR

#alp include <fresnel>

struct SSRValues {
    vec3 fallbackColor;

    vec2 uv;

    mat4 projection;
    mat4 view;

    float reflectionStrength;
    float jitter;

    int rayMarchCount;
    int binarySearchCount;
    float rayStep;
    float LLimiter;
    float minRayStep;
};

struct SSRResult {
    vec2 uv;
    vec3 normal;
    vec3 viewPos;
    vec3 reflected;
};

// Based on http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

vec2 ssr_binarySearch(
    inout vec3 dir,
    inout vec3 hitCoord,
    inout float dDepth,

    in sampler2D positionMap,
    in SSRValues values
) {
    float depth;

    vec4 projectedCoord;

    for (int i = 0; i < values.binarySearchCount; i++) {
        projectedCoord = values.projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(positionMap, projectedCoord.xy).z;

        dDepth = hitCoord.z - depth;

        dir *= 0.5;

        if (dDepth > 0.0) {
            hitCoord += dir;
        } else {
            hitCoord -= dir;
        }
    }

    projectedCoord = values.projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

    return projectedCoord.xy;
}

vec2 ssr_rayCast(
    vec3 dir,
    inout vec3 hitCoord,
    out float dDepth,

    in sampler2D positionMap,
    in SSRValues values
) {
    dir *= values.rayStep;

    for (int i = 0; i < values.rayMarchCount; i++) {
        hitCoord += dir;

        vec4 projectedCoord = values.projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = texture(positionMap, projectedCoord.xy).z;

        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0) {
            return ssr_binarySearch(
                dir, hitCoord, dDepth,
                positionMap, values
            );
        }
    }

    return vec2(-1.0);
}

#define _ssr_scale vec3(.8, .8, .8)
#define _ssr_k 19.19

vec3 _ssr_hash(vec3 a) {
    a = fract(a * _ssr_scale);
    a += dot(a, a.yxz + _ssr_k);
    return fract((a.xxy + a.yxx) * a.zyx);
}

SSRResult ssrGetResult(
    in sampler2D normalMap, // in view space
    in sampler2D positionMap, // in view space
    in SSRValues values
) {
    SSRResult ssr;

    ssr.normal = texture(normalMap, values.uv).xyz;
    ssr.viewPos = texture(positionMap, values.uv).xyz;

    vec3 worldPos = vec3(vec4(ssr.viewPos, 1.0) * inverse(values.view));
    vec3 jitt = _ssr_hash(worldPos) * values.jitter;

    // Reflection vector
    ssr.reflected = normalize(reflect(normalize(ssr.viewPos), normalize(ssr.normal)));

    // Ray cast
    vec3 hitPos = ssr.viewPos;
    float dDepth;
    ssr.uv = ssr_rayCast(
        jitt + ssr.reflected * max(-ssr.viewPos.z, values.minRayStep), hitPos, dDepth,
        positionMap, values
    );

    return ssr;
}

vec3 ssrGetColor(
    in sampler2D baseImage,
    in sampler2D normalMap, // in view space
    in sampler2D positionMap, // in view space
    in SSRValues values
) {
    if (values.reflectionStrength == 0) {
        return texture(baseImage, values.uv).xyz;
    }

    SSRResult ssr = ssrGetResult(normalMap, positionMap, values);

    float L = length(texture(positionMap, ssr.uv).xyz - ssr.viewPos);
    L = clamp(L * values.LLimiter, 0, 1);
    float error = 1 - L;

    float fresnel = invertFresnel(ssr.reflected, ssr.normal);

    vec3 baseColor = texture(baseImage, values.uv).xyz;
    vec3 color = (ssr.uv != vec2(-1.0)) ? texture(baseImage, ssr.uv).xyz * error * fresnel : values.fallbackColor;

    return mix(baseColor, color, values.reflectionStrength);
}

#undef _ssr_scale
#undef _ssr_k

#endif //ALGINE_MODULE_SSR

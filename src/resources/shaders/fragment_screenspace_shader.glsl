#version 330 core

#algdef

uniform sampler2D normalMap; // in view space
uniform sampler2D colorMap;
uniform sampler2D ssrValuesMap;
uniform sampler2D positionMap; // in view space
uniform mat4 projection, view;

uniform vec3 ssr_skyColor = vec3(0.0);
uniform int ssr_binarySearchCount = 10;
uniform int ssr_rayMarchCount = 30; // 60
uniform float ssr_step = 0.05; // 0.025
uniform float ssr_LLimiter = 0.1;
uniform float ssr_minRayStep = 0.2;

uniform vec3 bloom_vecThreshold = vec3(0.2126, 0.7152, 0.0722);
uniform float bloom_brightnessThreshold = 0.3;

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 bloom_fragColor;

// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

#define getPosition(texCoord) texture(positionMap, texCoord).xyz

vec2 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < ssr_binarySearchCount; i++) {
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = getPosition(projectedCoord.xy).z;
 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

    projectedCoord = projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec2(projectedCoord.xy);
}

vec2 rayCast(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    dir *= ssr_step;
    
    for (int i = 0; i < ssr_rayMarchCount; i++) {
        hitCoord += dir;

        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5; 

        float depth = getPosition(projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2 && dDepth <= 0.0) return binarySearch(dir, hitCoord, dDepth);
    }

    return vec2(-1.0);
}

#define scale vec3(.8, .8, .8)
#define k 19.19

vec3 hash(vec3 a) {
    a = fract(a * scale);
    a += dot(a, a.yxz + k);
    return fract((a.xxy + a.yxx)*a.zyx);
}

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
#define fresnelExp 5.0

float fresnel(vec3 direction, vec3 normal) {
    vec3 halfDirection = normalize(normal + direction);
    
    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0);
    float factor = 1.0 - pow(product, fresnelExp);
    
    return factor;
}

void ssr() {
    float reflectionStrength = texture(ssrValuesMap, texCoord).r;

    if (reflectionStrength == 0) {
        fragColor = texture(colorMap, texCoord).rgb;
        return;
    }

    vec3 normal = texture(normalMap, texCoord).xyz;
    vec3 viewPos = getPosition(texCoord);

    vec3 worldPos = vec3(vec4(viewPos, 1.0) * inverse(view));
    vec3 jitt = hash(worldPos) * texture(ssrValuesMap, texCoord).g;

    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));

    // Ray cast
    vec3 hitPos = viewPos;
    float dDepth; 
    vec2 coords = rayCast(jitt + reflected * max(-viewPos.z, ssr_minRayStep), hitPos, dDepth);

    float L = length(getPosition(coords) - viewPos);
    L = clamp(L * ssr_LLimiter, 0, 1);
    float error = 1 - L;

    float fresnel = fresnel(reflected, normal);
    
    vec3 color = texture(colorMap, coords.xy).rgb * error * fresnel;

    // fragColor = vec3(fresnel);
    // return;

    if (coords.xy != vec2(-1.0)) {
        fragColor = mix(texture(colorMap, texCoord), vec4(color, 1.0), reflectionStrength).rgb;
        return;
    }
    
    fragColor = mix(texture(colorMap, texCoord), vec4(ssr_skyColor, 1.0), reflectionStrength).rgb;
}

void main() {
    fragColor = texture(colorMap, texCoord).rgb;

    #ifdef ALGINE_SSR_MODE_ENABLED
    ssr();
    #endif
    
    #ifdef ALGINE_BLOOM_MODE_ENABLED
    if (dot(fragColor, bloom_vecThreshold) > bloom_brightnessThreshold) bloom_fragColor = fragColor;
    #endif
}
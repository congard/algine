// Algine SSR fragment shader

#version 330 core

uniform sampler2D normalMap; // in view space
uniform sampler2D colorMap;
uniform sampler2D ssrValuesMap;
uniform sampler2D positionMap; // in view space
uniform mat4 projection, view;

uniform vec3 skyColor = vec3(0.0);
uniform int binarySearchCount = 10;
uniform int rayMarchCount = 30; // 60
uniform float step = 0.05; // 0.025
uniform float LLimiter = 0.1;
uniform float minRayStep = 0.2;

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;

// SSR based on tutorial by Imanol Fotia
// http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

#define getPosition(texCoord) texture(positionMap, texCoord).xyz

vec2 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < binarySearchCount; i++) {
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
    dir *= step;
    
    for (int i = 0; i < rayMarchCount; i++) {
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

void main() {
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
    vec2 coords = rayCast(jitt + reflected * max(-viewPos.z, minRayStep), hitPos, dDepth);

    float L = length(getPosition(coords) - viewPos);
    L = clamp(L * LLimiter, 0, 1);
    float error = 1 - L;

    float fresnel = fresnel(reflected, normal);
    
    vec3 color = texture(colorMap, coords.xy).rgb * error * fresnel;

    // fragColor = vec3(fresnel);
    // return;

    if (coords.xy != vec2(-1.0)) {
        fragColor = mix(texture(colorMap, texCoord), vec4(color, 1.0), reflectionStrength).rgb;
        return;
    }
    
    fragColor = mix(texture(colorMap, texCoord), vec4(skyColor, 1.0), reflectionStrength).rgb;
}
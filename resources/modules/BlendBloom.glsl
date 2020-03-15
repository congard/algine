/**
 * Blend Bloom
 * It is module, not shader
 * Add it in your shader via pragma
 * algine include "modules/BlendBloom.glsl"
 */

#if defined(ALGINE_BLOOM_TYPE_ADD) || defined(ALGINE_BLOOM_TYPE_SCREEN)
#   define isBloomEnabled() true
 #else
#   define isBloomEnabled() false
#endif

// additive blending
vec3 blendBloomAdditive(in vec3 p1, in vec3 p2) {
    return p1 + p2;
}

// screen blending
vec3 blendBloomScreen(in vec3 p1, in vec3 p2) {
    return 1 - (1 - p1) * (1 - p2);
}

#ifdef ALGINE_BLOOM_TYPE_ADD
#   define blendBloom(p1, p2) blendBloomAdditive(p1, p2)
#elif defined(ALGINE_BLOOM_TYPE_SCREEN)
#   define blendBloom(p1, p2) blendBloomScreen(p1, p2)
#endif
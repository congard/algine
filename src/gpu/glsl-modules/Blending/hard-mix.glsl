#ifndef ALGINE_MODULE_BLENDING_HARD_MIX
#define ALGINE_MODULE_BLENDING_HARD_MIX

#alp include <Blending/vivid-light>

float blendHardMix(float base, float blend) {
    return (blendVividLight(base, blend) < 0.5f) ? 0.0f : 1.0f;
}

vec3 blendHardMix(vec3 base, vec3 blend) {
    return vec3(blendHardMix(base.r, blend.r), blendHardMix(base.g, blend.g), blendHardMix(base.b, blend.b));
}

vec3 blendHardMix(vec3 base, vec3 blend, float opacity) {
    return (blendHardMix(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_HARD_MIX

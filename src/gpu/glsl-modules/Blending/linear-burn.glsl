#ifndef ALGINE_MODULE_BLENDING_LINEAR_BURN
#define ALGINE_MODULE_BLENDING_LINEAR_BURN

float blendLinearBurn(float base, float blend) {
    // Note : Same implementation as BlendSubtractf
    return max(base + blend - 1.0f, 0.0f);
}

vec3 blendLinearBurn(vec3 base, vec3 blend) {
    // Note : Same implementation as BlendSubtract
    return max(base + blend - vec3(1.0f), vec3(0.0f));
}

vec3 blendLinearBurn(vec3 base, vec3 blend, float opacity) {
    return (blendLinearBurn(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_LINEAR_BURN

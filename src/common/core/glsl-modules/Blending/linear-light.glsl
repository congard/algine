#ifndef ALGINE_MODULE_BLENDING_LINEAR_LIGHT
#define ALGINE_MODULE_BLENDING_LINEAR_LIGHT

#alp include <Blending/linear-dodge>
#alp include <Blending/linear-burn>

float blendLinearLight(float base, float blend) {
    return blend < 0.5f ? blendLinearBurn(base, (2.0f * blend)) : blendLinearDodge(base, (2.0f * (blend - 0.5f)));
}

vec3 blendLinearLight(vec3 base, vec3 blend) {
    return vec3(blendLinearLight(base.r, blend.r), blendLinearLight(base.g, blend.g), blendLinearLight(base.b, blend.b));
}

vec3 blendLinearLight(vec3 base, vec3 blend, float opacity) {
    return (blendLinearLight(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_LINEAR_LIGHT

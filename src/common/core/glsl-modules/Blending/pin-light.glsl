#ifndef ALGINE_MODULE_BLENDING_PIN_LIGHT
#define ALGINE_MODULE_BLENDING_PIN_LIGHT

#alp include <Blending/lighten>
#alp include <Blending/darken>

float blendPinLight(float base, float blend) {
    return (blend < 0.5f) ? blendDarken(base, (2.0f * blend)) : blendLighten(base, (2.0f * (blend - 0.5f)));
}

vec3 blendPinLight(vec3 base, vec3 blend) {
    return vec3(blendPinLight(base.r, blend.r), blendPinLight(base.g, blend.g), blendPinLight(base.b, blend.b));
}

vec3 blendPinLight(vec3 base, vec3 blend, float opacity) {
    return (blendPinLight(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_PIN_LIGHT

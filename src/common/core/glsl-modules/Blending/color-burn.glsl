#ifndef ALGINE_MODULE_BLENDING_COLOR_BURN
#define ALGINE_MODULE_BLENDING_COLOR_BURN

float blendColorBurn(float base, float blend) {
    return (blend == 0.0f) ? blend:max((1.0f - ((1.0f - base) / blend)), 0.0f);
}

vec3 blendColorBurn(vec3 base, vec3 blend) {
    return vec3(blendColorBurn(base.r, blend.r), blendColorBurn(base.g, blend.g), blendColorBurn(base.b, blend.b));
}

vec3 blendColorBurn(vec3 base, vec3 blend, float opacity) {
    return (blendColorBurn(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_COLOR_BURN

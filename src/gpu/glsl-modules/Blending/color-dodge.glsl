#ifndef ALGINE_MODULE_BLENDING_COLOR_DODGE
#define ALGINE_MODULE_BLENDING_COLOR_DODGE

float blendColorDodge(float base, float blend) {
    return (blend == 1.0f) ? blend : min(base / (1.0f - blend), 1.0f);
}

vec3 blendColorDodge(vec3 base, vec3 blend) {
    return vec3(blendColorDodge(base.r, blend.r), blendColorDodge(base.g, blend.g), blendColorDodge(base.b, blend.b));
}

vec3 blendColorDodge(vec3 base, vec3 blend, float opacity) {
    return (blendColorDodge(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_COLOR_DODGE

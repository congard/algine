#ifndef ALGINE_MODULE_BLENDING_REFLECT
#define ALGINE_MODULE_BLENDING_REFLECT

float blendReflect(float base, float blend) {
    return (blend == 1.0f) ? blend : min(base * base / (1.0f - blend), 1.0f);
}

vec3 blendReflect(vec3 base, vec3 blend) {
    return vec3(blendReflect(base.r, blend.r), blendReflect(base.g, blend.g), blendReflect(base.b, blend.b));
}

vec3 blendReflect(vec3 base, vec3 blend, float opacity) {
    return (blendReflect(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_REFLECT

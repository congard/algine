#ifndef ALGINE_MODULE_BLENDING_ADD
#define ALGINE_MODULE_BLENDING_ADD

float blendAdd(float base, float blend) {
    return min(base + blend, 1.0f);
}

vec3 blendAdd(vec3 base, vec3 blend) {
    return min(base + blend, vec3(1.0f));
}

vec3 blendAdd(vec3 base, vec3 blend, float opacity) {
    return (blendAdd(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_ADD

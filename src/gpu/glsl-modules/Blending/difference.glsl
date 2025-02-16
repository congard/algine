#ifndef ALGINE_MODULE_BLENDING_DIFFERENCE
#define ALGINE_MODULE_BLENDING_DIFFERENCE

vec3 blendDifference(vec3 base, vec3 blend) {
    return abs(base - blend);
}

vec3 blendDifference(vec3 base, vec3 blend, float opacity) {
    return (blendDifference(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_DIFFERENCE

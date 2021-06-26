#ifndef ALGINE_MODULE_BLENDING_EXCLUSION
#define ALGINE_MODULE_BLENDING_EXCLUSION

vec3 blendExclusion(vec3 base, vec3 blend) {
    return base + blend - 2.0f * base * blend;
}

vec3 blendExclusion(vec3 base, vec3 blend, float opacity) {
    return (blendExclusion(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_EXCLUSION

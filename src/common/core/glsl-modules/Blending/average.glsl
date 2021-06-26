#ifndef ALGINE_MODULE_BLENDING_AVERAGE
#define ALGINE_MODULE_BLENDING_AVERAGE

vec3 blendAverage(vec3 base, vec3 blend) {
    return (base + blend) / 2.0f;
}

vec3 blendAverage(vec3 base, vec3 blend, float opacity) {
    return (blendAverage(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_AVERAGE

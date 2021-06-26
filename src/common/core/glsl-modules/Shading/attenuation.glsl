#ifndef ALGINE_MODULE_SHADING_ATTENUATION
#define ALGINE_MODULE_SHADING_ATTENUATION

// Based on: https://www.standardabweichung.de/code/javascript/webgl-glsl-cubic-attenuation
float attenuationCubic(float value, float max) {
    float attenuation = pow(clamp(1.0f - value * value / (max * max), 0.0f, 1.0f), 2.0f);
    return attenuation;
}

float attenuation(float distance, float kc, float kl, float kq) {
    float attenuation = 1.0f / (kc + kl * distance + kq * (distance * distance));
    return attenuation;
}

#endif //ALGINE_MODULE_SHADING_ATTENUATION

#ifndef ALGINE_MODULE_TONEMAPPING_EXPOSURE
#define ALGINE_MODULE_TONEMAPPING_EXPOSURE

vec3 tonemapExposure(vec3 x, float exposure) {
    return vec3(1.0f) - exp(-x * exposure);
}

float tonemapExposure(float x, float exposure) {
    return 1.0f - exp(-x * exposure);
}

#endif //ALGINE_MODULE_TONEMAPPING_EXPOSURE

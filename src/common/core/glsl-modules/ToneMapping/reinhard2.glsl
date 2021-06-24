#ifndef ALGINE_MODULE_TONEMAPPING_REINHARD2
#define ALGINE_MODULE_TONEMAPPING_REINHARD2

vec3 tonemapReinhard2(vec3 x) {
    const float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

float tonemapReinhard2(float x) {
    const float L_white = 4.0;

    return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

#endif //ALGINE_MODULE_TONEMAPPING_REINHARD2

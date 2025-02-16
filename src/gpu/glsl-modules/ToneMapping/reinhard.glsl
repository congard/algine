#ifndef ALGINE_MODULE_TONEMAPPING_REINHARD
#define ALGINE_MODULE_TONEMAPPING_REINHARD

vec3 tonemapReinhard(vec3 x) {
    return x / (1.0 + x);
}

float tonemapReinhard(float x) {
    return x / (1.0 + x);
}

#endif //ALGINE_MODULE_TONEMAPPING_REINHARD

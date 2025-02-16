#ifndef ALGINE_MODULE_TONEMAPPING_UNREAL
#define ALGINE_MODULE_TONEMAPPING_UNREAL

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
vec3 tonemapUnreal(vec3 x) {
    return x / (x + 0.155) * 1.019;
}

float tonemapUnreal(float x) {
    return x / (x + 0.155) * 1.019;
}

#endif //ALGINE_MODULE_TONEMAPPING_UNREAL

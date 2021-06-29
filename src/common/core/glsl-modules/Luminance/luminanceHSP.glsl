#ifndef ALGINE_MODULE_LUMINANCE_LUMINANCEHSP
#define ALGINE_MODULE_LUMINANCE_LUMINANCEHSP

// https://alienryderflex.com/hsp.html
float luminanceHSP(vec3 color) {
    return sqrt(0.299f * pow(color.r, 2) + 0.587f * pow(color.g, 2) + 0.114f * pow(color.b, 2));
}

#endif //ALGINE_MODULE_LUMINANCE_LUMINANCEHSP

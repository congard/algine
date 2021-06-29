#ifndef ALGINE_MODULE_LUMINANCE_LUMINANCE
#define ALGINE_MODULE_LUMINANCE_LUMINANCE

// https://en.wikipedia.org/wiki/Relative_luminance
float luminance(vec3 color) {
    return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
}

#endif //ALGINE_MODULE_LUMINANCE_LUMINANCE

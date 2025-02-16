#ifndef ALGINE_MODULE_LUMINANCE_LUMINANCEW3
#define ALGINE_MODULE_LUMINANCE_LUMINANCEW3

// https://www.w3.org/TR/AERT/#color-contrast
float luminanceW3(vec3 color) {
    return 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
}

#endif //ALGINE_MODULE_LUMINANCE_LUMINANCEW3

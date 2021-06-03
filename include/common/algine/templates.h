#ifndef ALGINE_TEMPLATES_H
#define ALGINE_TEMPLATES_H

#ifdef __ANDROID__
    #define enable_if_desktop(...)
    #define enable_if_android(...) __VA_ARGS__
#else
    #define enable_if_desktop(...) __VA_ARGS__
    #define enable_if_android(...)
#endif

#endif //ALGINE_TEMPLATES_H

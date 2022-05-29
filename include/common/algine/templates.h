#ifndef ALGINE_TEMPLATES_H
#define ALGINE_TEMPLATES_H

#ifdef __ANDROID__
    #define enable_if_desktop(...)
    #define enable_if_android(...) __VA_ARGS__
    #define is_android() true
    #define is_linux() false
    #define is_windows() false
#else
    #define enable_if_desktop(...) __VA_ARGS__
    #define enable_if_android(...)

    #define is_android() false

    #ifdef __linux__
        #define is_linux() true
        #define is_windows() false
    #else
        #define is_linux() false
        #define is_windows() true
    #endif
#endif

#endif //ALGINE_TEMPLATES_H

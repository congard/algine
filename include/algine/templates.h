#ifndef ALGINE_TEMPLATES_H
#define ALGINE_TEMPLATES_H

#include <tulz/macros.h>

#define implementVariadicCreate(T) \
template<typename...Args> \
static void create(Args&...args) { \
    T** arr[] = {&args...}; \
    for (usize i = 0; i < sizeof...(args); i++) \
        *arr[i] = new T(); \
}

#define implementVariadicDestroy(T) \
template<typename...Args> \
static void destroy(Args&...args) { \
    T** arr[] = {&args...}; \
    for (usize i = 0; i < sizeof...(args); i++) \
        deletePtr(*arr[i]); \
}

#ifdef __ANDROID__
    #define enable_if_desktop(...)
    #define enable_if_android(...) __VA_ARGS__
#else
    #define enable_if_desktop(...) __VA_ARGS__
    #define enable_if_android(...)
#endif

#endif //ALGINE_TEMPLATES_H

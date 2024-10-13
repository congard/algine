#ifndef ALGINE_TEMPLATES_H
#define ALGINE_TEMPLATES_H

#ifdef ALGINE_QT_PLATFORM
#include <QtGlobal>
#endif

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

#if is_windows()
    #ifdef ALGINE_BUILD_LIB
        #define AL_EXPORT __declspec(dllexport)
    #else
        #define AL_EXPORT __declspec(dllimport)
    #endif
#else
    #define AL_EXPORT
#endif

#define _AL_PLATFORM_STRINGIFY(x) #x
#define _AL_PLATFORM_STR(x) _AL_PLATFORM_STRINGIFY(x)

#define _AL_PLATFORM_VER_STRING(major, minor, patch) \
_AL_PLATFORM_STRINGIFY(major) "." _AL_PLATFORM_STRINGIFY(minor) "." _AL_PLATFORM_STRINGIFY(patch)

// platform

#ifdef ALGINE_QT_PLATFORM
#define PLATFORM_NAME "Qt"
#define PLATFORM_VERSION QT_VERSION_STR
#elif is_android()
#define PLATFORM_NAME "Android"
#define PLATFORM_VERSION _AL_PLATFORM_STR(__ANDROID_API__)
#else
#define PLATFORM_NAME "Desktop"
#define PLATFORM_VERSION "GLFW3"
#endif

// compiler

#ifdef __clang__
#define COMPILER_NAME "clang"
#define COMPILER_VERSION _AL_PLATFORM_VER_STRING(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#define COMPILER_NAME "gcc"
#define COMPILER_VERSION _AL_PLATFORM_VER_STRING(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif

#if __cplusplus == 202302L
#define CPP_VERSION_STR "C++23"
#elif __cplusplus == 202002L
#define CPP_VERSION_STR "C++20"
#elif __cplusplus == 201703L
#define CPP_VERSION_STR "C++17"
#else
#error "Unknown C++ version"
#endif

#endif //ALGINE_TEMPLATES_H

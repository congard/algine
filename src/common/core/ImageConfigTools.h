#ifndef ALGINE_IMAGECONFIGTOOLS_H
#define ALGINE_IMAGECONFIGTOOLS_H

#include <algine/core/texture/Texture.h>

#include <stdexcept>
#include <string>

#include "internal/ConfigStrings.h"

using namespace std;

namespace algine::Config {
#define format_uint(format) if (str == Config::format) return Texture::format;

inline uint stringToFormat(const string &str) {
    format_uint(DepthComponent)
    format_uint(DepthComponent16)
    format_uint(DepthComponent24)
    format_uint(DepthComponent32F)

    format_uint(DepthStencil)
    format_uint(Depth24Stencil8)
    format_uint(Depth32FStencil8)

    format_uint(Red)
    format_uint(RG)
    format_uint(RGB)
    format_uint(RGBA)

    format_uint(Red8)
    format_uint(RG8)
    format_uint(RGB8)
    format_uint(RGBA8)

    format_uint(Red16F)
    format_uint(RG16F)
    format_uint(RGB16F)
    format_uint(RGBA16F)

    format_uint(Red32F)
    format_uint(RG32F)
    format_uint(RGB32F)
    format_uint(RGBA32F)

    enable_if_desktop(
        format_uint(desktop_Red16)
        format_uint(desktop_RG16)
        format_uint(desktop_RGB16)
        format_uint(desktop_RGBA16)
    )

    throw runtime_error("Unsupported format '" + str + "'");
}

#undef format_uint

#define format_str(format) if (formatValue == Texture::format) return Config::format;

inline string formatToString(uint formatValue) {
    format_str(DepthComponent)
    format_str(DepthComponent16)
    format_str(DepthComponent24)
    format_str(DepthComponent32F)

    format_str(DepthStencil)
    format_str(Depth24Stencil8)
    format_str(Depth32FStencil8)

    format_str(Red)
    format_str(RG)
    format_str(RGB)
    format_str(RGBA)

    format_str(Red8)
    format_str(RG8)
    format_str(RGB8)
    format_str(RGBA8)

    format_str(Red16F)
    format_str(RG16F)
    format_str(RGB16F)
    format_str(RGBA16F)

    format_str(Red32F)
    format_str(RG32F)
    format_str(RGB32F)
    format_str(RGBA32F)

    enable_if_desktop(
        format_str(desktop_Red16)
        format_str(desktop_RG16)
        format_str(desktop_RGB16)
        format_str(desktop_RGBA16)
    )

    throw runtime_error("Unsupported format " + to_string(formatValue));
}

#undef format_str
}

#endif //ALGINE_IMAGECONFIGTOOLS_H

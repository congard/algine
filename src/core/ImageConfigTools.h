#ifndef ALGINE_IMAGECONFIGTOOLS_H
#define ALGINE_IMAGECONFIGTOOLS_H

#include <algine/core/texture/Texture.h>

#include <stdexcept>
#include <string>

#include "ConfigStrings.h"

using namespace std;

namespace algine {
namespace Config {
#define format_uint(format) if (str == Config::format) return Texture::format;

inline uint stringToFormat(const string &str) {
    format_uint(DepthComponent)
    format_uint(DepthStencil)

    format_uint(Red)
    format_uint(RG)
    format_uint(RGB)
    format_uint(RGBA)

    format_uint(Red16F)
    format_uint(RG16F)
    format_uint(RGB16F)
    format_uint(RGBA16F)

    format_uint(Red32F)
    format_uint(RG32F)
    format_uint(RGB32F)
    format_uint(RGBA32F)

    throw runtime_error("Unsupported format '" + str + "'");
}

#undef format_uint

#define format_str(format) if (formatValue == Texture::format) return Config::format;

inline string formatToString(uint formatValue) {
    format_str(DepthComponent)
    format_str(DepthStencil)

    format_str(Red)
    format_str(RG)
    format_str(RGB)
    format_str(RGBA)

    format_str(Red16F)
    format_str(RG16F)
    format_str(RGB16F)
    format_str(RGBA16F)

    format_str(Red32F)
    format_str(RG32F)
    format_str(RGB32F)
    format_str(RGBA32F)

    throw runtime_error("Unsupported format " + to_string(formatValue));
}

#undef format_str
}
}

#endif //ALGINE_IMAGECONFIGTOOLS_H

#ifndef ALGINE_TEXTURECONFIGMANIPULATOR_H
#define ALGINE_TEXTURECONFIGMANIPULATOR_H

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

#define param_uint(element) if (str == Config::element) return Texture::element;

inline uint stringToParamKey(const string &str) {
    param_uint(MinFilter)
    param_uint(MagFilter)
    param_uint(WrapU)
    param_uint(WrapV)
    param_uint(WrapW)

    throw runtime_error("Unsupported param key '" + str + "'");
}

inline uint stringToParamValue(const string &str) {
    param_uint(Nearest)
    param_uint(Linear)
    param_uint(ClampToEdge)
    param_uint(ClampToBorder)
    param_uint(Repeat)
    param_uint(MirroredRepeat)
    param_uint(MirrorClampToEdge)

    throw runtime_error("Unsupported param value '" + str + "'");
}

#undef param_uint

#define param_str(element) if (param == Texture::element) return Config::element;

inline string paramKeyToString(uint param) {
    param_str(MinFilter)
    param_str(MagFilter)
    param_str(WrapU)
    param_str(WrapV)
    param_str(WrapW)

    throw runtime_error("Unsupported param key " + to_string(param));
}

inline string paramValueToString(uint param) {
    param_str(Nearest)
    param_str(Linear)
    param_str(ClampToEdge)
    param_str(ClampToBorder)
    param_str(Repeat)
    param_str(MirroredRepeat)
    param_str(MirrorClampToEdge)

    throw runtime_error("Unsupported param value " + to_string(param));
}

#undef param_str

}
}

#endif //ALGINE_TEXTURECONFIGMANIPULATOR_H

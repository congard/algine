#ifndef ALGINE_TEXTURECONFIGTOOLS_H
#define ALGINE_TEXTURECONFIGTOOLS_H

#include <algine/core/texture/Texture.h>

#include <stdexcept>
#include <string>

#include "internal/ConfigStrings.h"

using namespace std;

namespace algine {
namespace Config {
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

#endif //ALGINE_TEXTURECONFIGTOOLS_H

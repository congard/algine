#ifndef ALGINE_CONFIGSTRINGS_H
#define ALGINE_CONFIGSTRINGS_H

#include <algine/core/DataType.h>

#include <string>

using namespace std;

#define constant(name, val) constexpr char name[] = val

namespace algine {
namespace Config {
constant(Type, "type");
constant(Texture2D, "texture2d");
constant(TextureCube, "textureCube");

constant(Width, "width");
constant(Height, "height");

constant(Format, "format");

// base formats
constant(DepthComponent, "depthComponent");
constant(DepthStencil, "depthStencil");

constant(Red, "red");
constant(RG, "rg");
constant(RGB, "rgb");
constant(RGBA, "rgba");

// some sized formats
constant(Red16F, "red16f");
constant(RG16F, "rg16f");
constant(RGB16F, "rgb16f");
constant(RGBA16F, "rgba16f");

constant(Red32F, "red32f");
constant(RG32F, "rg32f");
constant(RGB32F, "rgb32f");
constant(RGBA32F, "rgba32f");

constant(Params, "params");

// param keys
constant(MinFilter, "minFilter");
constant(MagFilter, "magFilter");
constant(WrapU, "wrapU");
constant(WrapV, "wrapV");
constant(WrapW, "wrapW");

// param values
constant(Nearest, "nearest");
constant(Linear, "linear");
constant(ClampToEdge, "clampToEdge");
constant(ClampToBorder, "clampToBorder");
constant(Repeat, "repeat");
constant(MirroredRepeat, "mirroredRepeat");
constant(MirrorClampToEdge, "mirrorClampToEdge");

constant(File, "file");
constant(Path, "path");
constant(Paths, "paths");
constant(DataType, "dataType");

// data types
constant(Byte, "byte");
constant(UnsignedByte, "ubyte");
constant(Short, "short");
constant(UnsignedShort, "ushort");
constant(Int, "int");
constant(UnsignedInt, "uint");
constant(Float, "float");
constant(HalfFloat, "halfFloat");
constant(Double, "double");

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

#define dataType_type(element) if (str == Config::element) return DataType::element;

inline algine::DataType stringToDataType(const string &str) {
    dataType_type(Byte)
    dataType_type(UnsignedByte)
    dataType_type(Short)
    dataType_type(UnsignedShort)
    dataType_type(Int)
    dataType_type(UnsignedInt)
    dataType_type(Float)
    dataType_type(HalfFloat)
    dataType_type(Double)

    throw runtime_error("Unsupported DataType '" + str + "'");
}

#undef dataType_type

#define dataType_str(element) if (dataType == DataType::element) return Config::element;

inline string dataTypeToString(algine::DataType dataType) {
    dataType_str(Byte)
    dataType_str(UnsignedByte)
    dataType_str(Short)
    dataType_str(UnsignedShort)
    dataType_str(Int)
    dataType_str(UnsignedInt)
    dataType_str(Float)
    dataType_str(HalfFloat)
    dataType_str(Double)

    throw runtime_error("Unsupported DataType " + to_string(static_cast<uint>(dataType)));
}

#undef dataType_str

}
}

#endif //ALGINE_CONFIGSTRINGS_H

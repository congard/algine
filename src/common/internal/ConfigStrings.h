#ifndef ALGINE_CONFIGSTRINGS_H
#define ALGINE_CONFIGSTRINGS_H

#include <algine/core/DataType.h>

#include <string>

using namespace std;

#define constant(name, val) constexpr char name[] = val

namespace algine::Config {
constant(Name, "name");
constant(Dump, "dump");

constant(Type, "type");
constant(Texture2D, "texture2d");
constant(TextureCube, "textureCube");
constant(Textures, "textures");
constant(Renderbuffer, "renderbuffer");

constant(Width, "width");
constant(Height, "height");

constant(ClassName, "className");

// may be used for example as attachments names
constant(Color, "color");
constant(Depth, "depth");
constant(Stencil, "stencil");
constant(Empty, "empty");

constant(Format, "format");

// base formats
constant(DepthComponent, "depthComponent");
constant(DepthStencil, "depthStencil");

constant(Red, "red");
constant(RG, "rg");
constant(RGB, "rgb");
constant(RGBA, "rgba");

// sized formats
constant(DepthComponent16, "depthComponent16");
constant(DepthComponent24, "depthComponent24");
constant(DepthComponent32F, "depthComponent32F");

constant(Depth24Stencil8, "depth24Stencil8");
constant(Depth32FStencil8, "depth32FStencil8");

constant(Red8, "red8");
constant(RG8, "rg8");
constant(RGB8, "rgb8");
constant(RGBA8, "rgba8");

constant(Red16F, "red16f");
constant(RG16F, "rg16f");
constant(RGB16F, "rgb16f");
constant(RGBA16F, "rgba16f");

constant(Red32F, "red32f");
constant(RG32F, "rg32f");
constant(RGB32F, "rgb32f");
constant(RGBA32F, "rgba32f");

constant(desktop_Red16, "desktop_red16");
constant(desktop_RG16, "desktop_rg16");
constant(desktop_RGB16, "desktop_rgb16");
constant(desktop_RGBA16, "desktop_rgba16");

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
constant(desktop_MirrorClampToEdge, "mirrorClampToEdge");

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

#endif //ALGINE_CONFIGSTRINGS_H

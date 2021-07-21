#ifndef ALGINE_TEXTUREPRIVATETOOLS_H
#define ALGINE_TEXTUREPRIVATETOOLS_H

#include <algine/core/texture/Texture.h>
#include <algine/core/DataType.h>

#include <algine/types.h>
#include <algine/gl.h>

namespace algine::TexturePrivateTools {
inline uint getTexParam(const uint target, const uint name) {
    constexpr uint mipLevel = 0;

    int value;
    glGetTexLevelParameteriv(target, mipLevel, name, &value);

    return value;
}

struct TextureDataInfo {
    uint format;
    DataType type;
};

inline constexpr TextureDataInfo getDataInfo(uint dataFormat) {
    // last 3 params for glTexImage2D are never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml

    // GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
    // GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    enable_if_desktop(
    uint format {};

    switch (dataFormat) {
        case Texture::DepthComponent:
        case Texture::DepthComponent16:
        case Texture::DepthComponent24:
        case Texture::DepthComponent32F:
            format = Texture::DepthComponent;
            break;
        case Texture::Red8UI:
        case Texture::RG8UI:
        case Texture::RGB8UI:
        case Texture::RGBA8UI:
        case Texture::Red8I:
        case Texture::RG8I:
        case Texture::RGB8I:
        case Texture::RGBA8I:
        case Texture::Red16UI:
        case Texture::RG16UI:
        case Texture::RGB16UI:
        case Texture::RGBA16UI:
        case Texture::Red16I:
        case Texture::RG16I:
        case Texture::RGB16I:
        case Texture::RGBA16I:
        case Texture::Red32UI:
        case Texture::RG32UI:
        case Texture::RGB32UI:
        case Texture::RGBA32UI:
        case Texture::Red32I:
        case Texture::RG32I:
        case Texture::RGB32I:
        case Texture::RGBA32I:
            format = Texture::RedInteger;
            break;
        default:
            format = Texture::Red;
            break;
    }

    return {format, DataType::Byte}
    );

    // android (OpenGL ES)

    // GL_INVALID_OPERATION is generated if the combination of internalFormat, format and type
    // is not one of those in the tables above.
    // https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml
    switch (dataFormat) {
        case Texture::DepthComponent16:
            return {Texture::DepthComponent, DataType::UnsignedShort};
        case Texture::DepthComponent24:
            return {Texture::DepthComponent, DataType::UnsignedInt};
        case Texture::DepthComponent32F:
            return {Texture::DepthComponent, DataType::Float};
        case Texture::Depth24Stencil8:
            return {Texture::DepthStencil, static_cast<DataType>(GL_UNSIGNED_INT_24_8)};
        case Texture::Depth32FStencil8:
            return {Texture::DepthStencil, static_cast<DataType>(GL_FLOAT_32_UNSIGNED_INT_24_8_REV)};
        case Texture::RGB:
            return {Texture::RGB, DataType::UnsignedByte};
        case Texture::RGBA:
            return {Texture::RGBA, DataType::UnsignedByte};
        case Texture::Red8:
            return {Texture::Red, DataType::UnsignedByte};
        case Texture::Red16F:
            return {Texture::Red, DataType::HalfFloat};
        case Texture::Red32F:
            return {Texture::Red, DataType::Float};
        case Texture::RG8:
            return {Texture::RG, DataType::UnsignedByte};
        case Texture::RG16F:
            return {Texture::RG, DataType::HalfFloat};
        case Texture::RG32F:
            return {Texture::RG, DataType::Float};
        case Texture::RGB8:
            return {Texture::RGB, DataType::UnsignedByte};
        case Texture::RGB16F:
            return {Texture::RGB, DataType::HalfFloat};
        case Texture::RGB32F:
            return {Texture::RGB, DataType::Float};
        case Texture::RGBA8:
            return {Texture::RGBA, DataType::UnsignedByte};
        case Texture::RGBA16F:
            return {Texture::RGBA, DataType::HalfFloat};
        case Texture::RGBA32F:
            return {Texture::RGBA, DataType::Float};
        case Texture::Red8UI:
            return {Texture::RedInteger, DataType::UnsignedByte};
        case Texture::RG8UI:
            return {Texture::RGInteger, DataType::UnsignedByte};
        case Texture::RGB8UI:
            return {Texture::RGBInteger, DataType::UnsignedByte};
        case Texture::RGBA8UI:
            return {Texture::RGBAInteger, DataType::UnsignedByte};
        case Texture::Red8I:
            return {Texture::RedInteger, DataType::Byte};
        case Texture::RG8I:
            return {Texture::RGInteger, DataType::Byte};
        case Texture::RGB8I:
            return {Texture::RGBInteger, DataType::Byte};
        case Texture::RGBA8I:
            return {Texture::RGBAInteger, DataType::Byte};
        case Texture::Red16UI:
            return {Texture::RedInteger, DataType::UnsignedShort};
        case Texture::RG16UI:
            return {Texture::RGInteger, DataType::UnsignedShort};
        case Texture::RGB16UI:
            return {Texture::RGBInteger, DataType::UnsignedShort};
        case Texture::RGBA16UI:
            return {Texture::RGBAInteger, DataType::UnsignedShort};
        case Texture::Red16I:
            return {Texture::RedInteger, DataType::Short};
        case Texture::RG16I:
            return {Texture::RGInteger, DataType::Short};
        case Texture::RGB16I:
            return {Texture::RGBInteger, DataType::Short};
        case Texture::RGBA16I:
            return {Texture::RGBAInteger, DataType::Short};
        case Texture::Red32UI:
            return {Texture::RedInteger, DataType::UnsignedInt};
        case Texture::RG32UI:
            return {Texture::RGInteger, DataType::UnsignedInt};
        case Texture::RGB32UI:
            return {Texture::RGBInteger, DataType::UnsignedInt};
        case Texture::RGBA32UI:
            return {Texture::RGBAInteger, DataType::UnsignedInt};
        case Texture::Red32I:
            return {Texture::RedInteger, DataType::Int};
        case Texture::RG32I:
            return {Texture::RGInteger, DataType::Int};
        case Texture::RGB32I:
            return {Texture::RGBInteger, DataType::Int};
        case Texture::RGBA32I:
            return {Texture::RGBAInteger, DataType::Int};
        default: {
            throw std::runtime_error("Unsupported internal format " + std::to_string(dataFormat));
        }
    }
}
}

#endif //ALGINE_TEXTUREPRIVATETOOLS_H

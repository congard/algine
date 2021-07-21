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

inline constexpr Texture::FormatInfo getDataInfo(uint dataFormat) {
    // last 3 params for glTexImage2D are never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml

    // GL_INVALID_OPERATION is generated if internalformat is GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
    // GL_DEPTH_COMPONENT24, or GL_DEPTH_COMPONENT32F, and format is not GL_DEPTH_COMPONENT
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    enable_if_desktop(
    int format {};

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
    return Texture::getFormatInfo(dataFormat);
}
}

#endif //ALGINE_TEXTUREPRIVATETOOLS_H

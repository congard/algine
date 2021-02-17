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
    enable_if_desktop(return {
        dataFormat == Texture::DepthComponent ? Texture::DepthComponent : Texture::Red, DataType::Byte
    });

    // android (OpenGL ES)

    // GL_INVALID_OPERATION is generated if the combination of internalFormat, format and type
    // is not one of those in the tables above.
    // https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml
    switch (dataFormat) {
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
        default: {
            throw std::runtime_error("Unsupported internal format " + std::to_string(dataFormat));
        }
    }
}
}

#endif //ALGINE_TEXTUREPRIVATETOOLS_H

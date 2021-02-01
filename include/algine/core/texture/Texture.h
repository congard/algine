#ifndef ALGINE_TEXTURE_H
#define ALGINE_TEXTURE_H

#include <algine/core/texture/TextureCreateInfo.h>
#include <algine/core/DataType.h>
#include <algine/core/Object.h>

#include <algine/templates.h>
#include <algine/types.h>
#include <algine/gl.h>

#include <map>
#include <string>

namespace algine {
class Texture: public Object {
    friend class Engine;

public:
    enum Params {
        MinFilter = GL_TEXTURE_MIN_FILTER,
        MagFilter = GL_TEXTURE_MAG_FILTER,
        WrapU = GL_TEXTURE_WRAP_S,
        WrapV = GL_TEXTURE_WRAP_T,
        WrapW = GL_TEXTURE_WRAP_R
    };

    enum Values {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,

        enable_if_desktop(
            desktop_MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
        )
    };

    enum BaseFormats {
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL,
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA
    };

    enum SizedFormats {
        Red8 = GL_R8,
        RG8 = GL_RG8,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        Red16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        Red32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        enable_if_desktop(
            desktop_Red16 = GL_R16,
            desktop_RG16 = GL_RG16,
            desktop_RGBA16 = GL_RGBA16
        )
    };

public:
    Texture();
    ~Texture();

    void bind() const;
    void unbind() const;
    void use(uint slot) const; // activate + bind
    void setParams(const std::map<uint, uint> &params);
    void setParams(const std::map<uint, float> &params);
    void applyTextureCreateInfo(const TextureCreateInfo &createInfo);

    /**
     * LOD - level of detail
     * Level 0 is the base image level. Level n is the nth mipmap reduction image
     */
    void setLOD(uint lod);

    /**
     * Specifies the number of color components and format of the texture
     * @param format
     */
    void setFormat(uint format);

    void setWidth(uint width);
    void setHeight(uint height);
    void setDimensions(uint width, uint height);

    /// updates width / height, lod, format
    virtual void update() = 0;

    uint getLOD() const;
    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;
    uint getId() const;

    virtual uint getActualFormat() const = 0;
    virtual uint getActualWidth() const = 0;
    virtual uint getActualHeight() const = 0;

protected:
    uint m_target = 0; // texture 2d, texture cube etc
    uint m_id = 0;
    uint m_lod = 0;
    uint m_format = RGB16F;
    uint m_width = 512, m_height = 512;

protected:
    explicit Texture(uint target);
    void texFromFile(const std::string &path, uint target, DataType dataType = DataType::UnsignedByte, bool flipImage = true);
};
}

#endif //ALGINE_TEXTURE_H

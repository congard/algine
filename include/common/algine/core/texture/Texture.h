#ifndef ALGINE_TEXTURE_H
#define ALGINE_TEXTURE_H

#include <algine/core/context/ContextObject.h>
#include <algine/core/texture/TextureCreateInfo.h>
#include <algine/core/texture/TextureFileInfo.h>
#include <algine/core/DataType.h>
#include <algine/core/Object.h>

#include <algine/templates.h>
#include <algine/types.h>
#include <algine/gl.h>

#include <map>
#include <string>

namespace algine {
AL_CONTEXT_OBJECT(Texture) {
public:
    enum Param {
        MinFilter = GL_TEXTURE_MIN_FILTER,
        MagFilter = GL_TEXTURE_MAG_FILTER,
        WrapU = GL_TEXTURE_WRAP_S,
        WrapV = GL_TEXTURE_WRAP_T,
        WrapW = GL_TEXTURE_WRAP_R
    };

    enum Value {
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

    enum Format {
        // base formats
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL,
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        RedInteger = GL_RED_INTEGER,
        RGInteger = GL_RG_INTEGER,
        RGBInteger = GL_RGB_INTEGER,
        RGBAInteger = GL_RGBA_INTEGER,

        // sized formats
        DepthComponent16 = GL_DEPTH_COMPONENT16,
        DepthComponent24 = GL_DEPTH_COMPONENT24,
        DepthComponent32F = GL_DEPTH_COMPONENT32F,
        Depth24Stencil8 = GL_DEPTH24_STENCIL8,
        Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
        Red8 = GL_R8,
        RG8 = GL_RG8,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        Red8UI = GL_R8UI,
        RG8UI = GL_RG8UI,
        RGB8UI = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,
        Red8I = GL_R8I,
        RG8I = GL_RG8I,
        RGB8I = GL_RGB8I,
        RGBA8I = GL_RGBA8I,
        Red16UI = GL_R16UI,
        RG16UI = GL_RG16UI,
        RGB16UI = GL_RGB16UI,
        RGBA16UI = GL_RGBA16UI,
        Red16I = GL_R16I,
        RG16I = GL_RG16I,
        RGB16I = GL_RGB16I,
        RGBA16I = GL_RGBA16I,
        Red32UI = GL_R32UI,
        RG32UI = GL_RG32UI,
        RGB32UI = GL_RGB32UI,
        RGBA32UI = GL_RGBA32UI,
        Red32I = GL_R32I,
        RG32I = GL_RG32I,
        RGB32I = GL_RGB32I,
        RGBA32I = GL_RGBA32I,
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
            desktop_RGB16 = GL_RGB16,
            desktop_RGBA16 = GL_RGBA16
        )
    };

public:
    struct FormatInfo {
        int baseFormat;
        DataType dataType;
    };

public:
    explicit Texture(Object *parent = defaultParent());
    ~Texture() override;

    void bind() const;
    void unbind() const;
    void use(uint slot) const; // activateSlot + bind
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

    uint getParam(uint param) const;

    virtual uint getActualFormat() const = 0;
    virtual uint getActualWidth() const = 0;
    virtual uint getActualHeight() const = 0;

    static void activateSlot(uint slot);
    static FormatInfo getFormatInfo(uint format);

protected:
    uint m_target = 0; // texture 2d, texture cube etc
    uint m_lod = 0;
    uint m_format = RGB16F;
    uint m_width = 512;
    uint m_height = 512;

protected:
    explicit Texture(uint target, Object *parent);
    void texFromFile(uint target, const TextureFileInfo &info);
    static uint getTexParam(uint target, uint name);
};
}

#endif //ALGINE_TEXTURE_H

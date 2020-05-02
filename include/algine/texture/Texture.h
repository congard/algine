#ifndef ALGINE_TEXTURE_H
#define ALGINE_TEXTURE_H

#include <GL/glew.h>
#include <map>

#include <algine/types.h>
#include <algine/texture/TextureCreateInfo.h>

namespace algine {
class Texture {
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
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
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
        Red16 = GL_R16,
        RG8 = GL_RG8,
        RG16 = GL_RG16,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGBA16 = GL_RGBA16,
        Red16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        Red32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
    };

    Texture();
    ~Texture();

    void bind() const;
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
    void setWidthHeight(uint width, uint height);

    /// updates width / height, lod, format
    virtual void update() = 0;

    // TODO: shader: rename > unbind
    void unbind() const;

    uint getLOD() const;
    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;
    uint getId() const;

public:
    uint target = 0; // texture 2d, texture cube etc
    uint id = 0;
    uint
        lod = 0,
        format = RGB16F,
        width = 512,
        height = 512;

protected:
    explicit Texture(uint target);
    void texFromFile(const std::string &path, uint target, uint dataType = GL_UNSIGNED_BYTE, bool flipImage = true);
};
}

#endif //ALGINE_TEXTURE_H
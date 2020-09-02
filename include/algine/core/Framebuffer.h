#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/types.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/Renderbuffer.h>
#include <algine/core/texture/PixelData.h>
#include <algine/templates.h>
#include <vector>

namespace algine {
class Framebuffer {
    friend class Engine;

public:
    enum Attachments {
        ColorAttachmentZero = GL_COLOR_ATTACHMENT0,
        DepthAttachment = GL_DEPTH_ATTACHMENT,
        StencilAttachment = GL_STENCIL_ATTACHMENT,
        DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT,
        EmptyAttachment = GL_NONE
    };

    enum BuffersMasks {
        ColorBuffer = 0x00004000,
        DepthBuffer = 0x00000100,
        StencilBuffer = 0x00000400
    };

public:
    Framebuffer();
    ~Framebuffer();

    void bind() const;
    void unbind() const;
    void attachTexture(const Texture2D *texture, uint attachment);
    void attachTexture(const TextureCube *texture, uint attachment);
    void attachRenderbuffer(const Renderbuffer *renderbuffer, uint attachment);
    void addAttachmentsList(const std::vector<uint> &list = {ColorAttachmentZero});
    void addOutput(uint outIndex, uint attachment);
    void removeOutput(uint outIndex, bool optimizeList = true);
    void optimizeAttachmentsList();
    void update();
    void clear(uint buffersMask);
    void clearColorBuffer();
    void clearDepthBuffer();
    void clearStencilBuffer();

    void setActiveAttachmentsList(uint index);
    void setAttachmentsList(uint index, const std::vector<uint> &list);
    void setAttachmentsLists(const std::vector<std::vector<uint>> &lists);

    PixelData getPixels2D(uint mode, uint x, uint y, uint width, uint height, int format = -1) const;
    PixelData getAllPixels2D(uint attachment, int format = -1) const;
    PixelData getAllPixelsCube(TextureCube::Face face, uint attachment, int format = -1) const;

    uint getActiveAttachmentsList() const;
    uint getId() const;
    std::vector<uint> getAttachmentsList(uint index) const;
    std::vector<std::vector<uint>> getAttachmentsLists() const;

    static void setClearColor(float red, float green, float blue, float alpha = 1.0f);
    static void setClearDepth(float depth);
    static void setClearStencil(int s);

    implementVariadicCreate(Framebuffer)
    implementVariadicDestroy(Framebuffer)

protected:
    uint m_id;
    uint m_activeList;
    std::vector<std::vector<uint>> m_attachmentsLists;
};

}

#endif /* ALGINE_FRAMEBUFFER_H */
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

    Framebuffer();
    ~Framebuffer();

    void bind();
    void attachTexture(const Texture2D *texture, uint attachment);
    void attachTexture(const TextureCube *texture, uint attachment);
    void attachRenderbuffer(const Renderbuffer *renderbuffer, uint attachment);
    void addAttachmentsList(const std::vector<uint> &list = {ColorAttachmentZero});
    void addOutput(uint outIndex, uint attachment);
    void removeOutput(uint outIndex, bool optimizeList = true);
    void optimizeAttachmentsList();
    void update();
    void unbind();

    void setActiveAttachmentsList(uint index);
    void setAttachmentsList(uint index, const std::vector<uint> &list);
    void setAttachmentsLists(const std::vector<std::vector<uint>> &lists);

    PixelData getPixels2D(uint mode, uint x, uint y, uint width, uint height, int format = -1);
    PixelData getAllPixels2D(uint attachment, int format = -1);
    PixelData getAllPixelsCube(uint face, uint attachment, int format = -1);

    uint getActiveAttachmentsList() const;
    uint getId() const;
    std::vector<uint> getAttachmentsList(uint index) const;
    std::vector<std::vector<uint>> getAttachmentsLists() const;

    implementVariadicCreate(Framebuffer)
    implementVariadicDestroy(Framebuffer)

protected:
    uint m_id = 0;
    uint m_activeList = 0;
    std::vector<std::vector<uint>> m_attachmentsLists = {std::vector<uint> {ColorAttachmentZero}};
};

}

#endif /* ALGINE_FRAMEBUFFER_H */
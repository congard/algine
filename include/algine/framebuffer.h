#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/types.h>
#include <algine/texture.h>
#include <algine/renderbuffer.h>
#include <algine/templates.h>
#include <vector>

#define bindFramebuffer(framebuffer) glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)

namespace algine {
class Framebuffer {
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

    uint getActiveAttachmentsList() const;
    uint getId() const;
    std::vector<uint> getAttachmentsList(uint index) const;
    std::vector<std::vector<uint>> getAttachmentsLists() const;

    implementVariadicCreate(Framebuffer)
    implementVariadicDestroy(Framebuffer)

    // TODO: remove
    static void create(uint *id);
    static void attachTexture2D(const uint &textureId, const uint &colorAttachment);
    static void destroy(uint *id);

protected:
    uint m_id = 0;
    uint m_activeList = 0;
    std::vector<std::vector<uint>> m_attachmentsLists;
};

}

#endif /* ALGINE_FRAMEBUFFER_H */
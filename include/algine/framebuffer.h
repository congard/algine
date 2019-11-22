#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/types.h>
#include <algine/texture.h>
#include <algine/renderbuffer.h>
#include <algine/templates.h>

#define bindFramebuffer(framebuffer) glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)

namespace algine {
class Framebuffer {
public:
    enum Attachments {
        ColorAttachmentZero = GL_COLOR_ATTACHMENT0,
        DepthAttachment = GL_DEPTH_ATTACHMENT,
        StencilAttachment = GL_STENCIL_ATTACHMENT,
        DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT
    };

    Framebuffer();
    ~Framebuffer();

    void bind();
    void attachTexture(const Texture2D *texture, uint attachment);
    void attachTexture(const TextureCube *texture, uint attachment);
    void attachRenderbuffer(const Renderbuffer *renderbuffer, uint attachment);
    void unbind();

    uint getId() const;

    implementVariadicCreate(Framebuffer)
    implementVariadicDestroy(Framebuffer)

    // TODO: remove
    static void create(uint *id);
    static void attachTexture2D(const uint &textureId, const uint &colorAttachment);
    static void destroy(uint *id);

protected:
    uint id = 0;
};

}

#endif /* ALGINE_FRAMEBUFFER_H */
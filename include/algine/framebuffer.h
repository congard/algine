#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/types.h>

#define bindFramebuffer(framebuffer) glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)

namespace algine {
struct Framebuffer {
    static void create(uint *id);

    static void create(uint *id, const uint &count);

    static void attachTexture2D(const uint &textureId, const uint &colorAttachment);

    static void attachRenderbuffer(const uint &attachment, const uint &renderbuffer);

    static void destroy(uint *id);

    static void destroy(uint *id, const uint &count);
};

}

#endif /* ALGINE_FRAMEBUFFER_H */
#ifndef ALGINE_FRAMEBUFFER_CPP
#define ALGINE_FRAMEBUFFER_CPP

#include <GL/glew.h>
#include <algine/types.h>

namespace algine {
#define bindFramebuffer(framebuffer) glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)

struct Framebuffer {
    static void create(uint *id) {
        glGenFramebuffers(1, id);
    }

    static void create(uint *id, const uint &count) {
        glGenFramebuffers(count, id);
    }

    static void attachTexture2D(const uint &textureId, const uint &colorAttachment) {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, textureId, 0);
    }

    static void attachRenderbuffer(const uint &attachment, const uint &renderbuffer) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
    }

    static void destroy(uint *id) {
        glDeleteFramebuffers(1, id);
    }

    static void destroy(uint *id, const uint &count) {
        glDeleteFramebuffers(count, id);
    }
};

}

#endif /* ALGINE_FRAMEBUFFER_CPP */
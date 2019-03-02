#ifndef ALGINE_FRAMEBUFFER_CPP
#define ALGINE_FRAMEBUFFER_CPP

#include <GL/glew.h>

namespace algine {
    #define bindFramebuffer(framebuffer) glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)

    struct Framebuffer {
        static void create(GLuint *id) {
            glGenFramebuffers(1, id);
        }

        static void create(GLuint *id, GLuint count) {
            glGenFramebuffers(count, id);
        }

        static void attachTexture2D(GLuint textureId, GLuint colorAttachment) {
            glBindTexture(GL_TEXTURE_2D, textureId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, textureId, 0);
        }

        static void destroy(GLuint *id) {
            glDeleteFramebuffers(1, id);
        }
    };
}

#endif /* ALGINE_FRAMEBUFFER_CPP */
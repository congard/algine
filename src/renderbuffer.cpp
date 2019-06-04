#ifndef ALGINE_RENDERBUFFER_CPP
#define ALGINE_RENDERBUFFER_CPP

#include <GL/glew.h>
#include <algine/types.h>

namespace algine {
#define bindRenderbuffer(renderbuffer) glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer)

struct Renderbuffer {
    static void create(uint *id) {
        glGenRenderbuffers(1, id);
    }

    static void create(uint *id, const uint &count) {
        glGenRenderbuffers(count, id);
    }

    static void setupStorage(const uint &internalformat, const uint &width, const uint &height) {
        glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    }

    static void destroy(uint *id) {
        glDeleteRenderbuffers(1, id);
    }

    static void destroy(uint *id, const uint &count) {
        glDeleteRenderbuffers(count, id);
    }
};

}

#endif /* ALGINE_RENDERBUFFER_CPP */
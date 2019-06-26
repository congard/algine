#ifndef ALGINE_RENDERBUFFER_H
#define ALGINE_RENDERBUFFER_H

#include <algine/types.h>

#define bindRenderbuffer(renderbuffer) glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer)

namespace algine {
struct Renderbuffer {
    static void create(uint *id);

    static void create(uint *id, const uint &count);

    static void setupStorage(const uint &internalformat, const uint &width, const uint &height);

    static void destroy(uint *id);

    static void destroy(uint *id, const uint &count);
};

}

#endif /* ALGINE_RENDERBUFFER_H */
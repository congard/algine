#include <algine/renderbuffer.h>
#include <GL/glew.h>

namespace algine {
void Renderbuffer::create(uint *id) {
    glGenRenderbuffers(1, id);
}

void Renderbuffer::create(uint *id, const uint &count) {
    glGenRenderbuffers(count, id);
}

void Renderbuffer::setupStorage(const uint &internalformat, const uint &width, const uint &height) {
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
}

void Renderbuffer::destroy(uint *id) {
    glDeleteRenderbuffers(1, id);
}

void Renderbuffer::destroy(uint *id, const uint &count) {
    glDeleteRenderbuffers(count, id);
}
};
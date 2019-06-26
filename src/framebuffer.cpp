#include <algine/framebuffer.h>
#include <GL/glew.h>

namespace algine {
void Framebuffer::create(uint *id) {
    glGenFramebuffers(1, id);
}

void Framebuffer::create(uint *id, const uint &count) {
    glGenFramebuffers(count, id);
}

void Framebuffer::attachTexture2D(const uint &textureId, const uint &colorAttachment) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, textureId, 0);
}

void Framebuffer::attachRenderbuffer(const uint &attachment, const uint &renderbuffer) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer);
}

void Framebuffer::destroy(uint *id) {
    glDeleteFramebuffers(1, id);
}

void Framebuffer::destroy(uint *id, const uint &count) {
    glDeleteFramebuffers(count, id);
}
}
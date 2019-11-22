#include <algine/framebuffer.h>
#include <GL/glew.h>

namespace algine {
Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &id);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &id);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

// TODO: not marked static because will be implemented "secure operations" check, which needs this pointer

void Framebuffer::attachTexture(const Texture2D *const texture, const uint attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getId(), 0);
}

void Framebuffer::attachTexture(const TextureCube *const texture, const uint attachment) {
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);
}

void Framebuffer::attachRenderbuffer(const Renderbuffer *const renderbuffer, const uint attachment) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getId());
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint Framebuffer::getId() const {
    return id;
}

void Framebuffer::create(uint *id) {
    glGenFramebuffers(1, id);
}

void Framebuffer::attachTexture2D(const uint &textureId, const uint &colorAttachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, textureId, 0);
}

void Framebuffer::destroy(uint *id) {
    glDeleteFramebuffers(1, id);
}
}
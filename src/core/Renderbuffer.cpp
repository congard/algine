#include <algine/core/Renderbuffer.h>
#include <GL/glew.h>

namespace algine {
Renderbuffer::Renderbuffer() {
    glGenRenderbuffers(1, &id);
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &id);
}

void Renderbuffer::bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, id);
}

void Renderbuffer::setFormat(const uint _format) {
    format = _format;
}

void Renderbuffer::setWidth(const uint _width) {
    width = _width;
}

void Renderbuffer::setHeight(const uint _height) {
    height = _height;
}

void Renderbuffer::setWidthHeight(const uint _width, const uint _height) {
    width = _width;
    height = _height;
}

void Renderbuffer::update() {
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

void Renderbuffer::unbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

uint Renderbuffer::getFormat() const {
    return format;
}

uint Renderbuffer::getWidth() const {
    return width;
}

uint Renderbuffer::getHeight() const {
    return height;
}

uint Renderbuffer::getId() const {
    return id;
}
};
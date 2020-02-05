#include <algine/Buffer.h>

namespace algine {

Buffer::Buffer() {
    glGenBuffers(1, &m_id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const {
    glBindBuffer(m_target, m_id);
}

void Buffer::unbind() const {
    glBindBuffer(m_target, 0);
}

void Buffer::setData(const uint size, const void *data, const uint usage) {
    glBufferData(m_target, size, data, usage);
}

uint Buffer::getId() const {
    return m_id;
}

Buffer::Buffer(const uint target): Buffer() {
    this->m_target = target;
}
}
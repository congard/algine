#include <algine/core/buffers/BufferReader.h>

using namespace tulz;

namespace algine {
BufferReader::BufferReader() = default;

BufferReader::BufferReader(Buffer *buffer)
    : m_buffer(buffer) {}

void BufferReader::begin() {
    m_buffer->bind();
}

void BufferReader::end() {
    m_buffer->unbind();
}

Array<byte> BufferReader::read(uint offset, uint size) {
    return m_buffer->getData(offset, size);
}

void BufferReader::setBuffer(Buffer *buffer) {
    m_buffer = buffer;
}

Buffer* BufferReader::getBuffer() const {
    return m_buffer;
}
}

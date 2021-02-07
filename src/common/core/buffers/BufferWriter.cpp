#include <algine/core/buffers/BufferWriter.h>

using namespace std;

namespace algine {
BufferWriter::BufferWriter() = default;

BufferWriter::BufferWriter(Buffer *buffer)
    : m_buffer(buffer) {}

void BufferWriter::begin() {
    m_buffer->bind();
}

void BufferWriter::end() {
    m_buffer->unbind();
}

void BufferWriter::write(uint offset, uint size, const void *data) {
    m_buffer->updateData(offset, size, data);
}

void BufferWriter::setBuffer(Buffer *buffer) {
    m_buffer = buffer;
}

Buffer* BufferWriter::getBuffer() const {
    return m_buffer;
}
}

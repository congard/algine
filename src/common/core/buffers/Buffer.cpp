#include <algine/core/buffers/Buffer.h>
#include <algine/platform.h>
#include <algine/gl.h>

using namespace algine;
using namespace std;
using namespace tulz;

namespace algine {
Buffer::Buffer() = default;

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const {
    commitBinding();
    glBindBuffer(m_target, m_id);
}

void Buffer::unbind() const {
    checkBinding();
    commitUnbinding();
    glBindBuffer(m_target, 0);
}

void Buffer::setData(uint size, const void *data, uint usage) {
    checkBinding();
    glBufferData(m_target, size, data, usage);
}

void Buffer::updateData(uint offset, uint size, const void *data) {
    checkBinding();
    glBufferSubData(m_target, offset, size, data);
}

Array<byte> Buffer::getData(uint offset, uint size) {
    checkBinding();

    if (size == 0)
        return {};

    tulz::Array<byte> array(size);

    enable_if_desktop(
        glGetBufferSubData(m_target, offset, size, array.array());
    )

    enable_if_android(
        auto ptr = mapData(offset, size, MapMode::Read);

        if (ptr != nullptr) {
            memcpy(array.array(), ptr, size);
        }

        unmapData();
    )

    return array;
}

void* Buffer::mapData(uint offset, uint size, uint access) {
    checkBinding();
    return glMapBufferRange(m_target, offset, size, access);
}

bool Buffer::unmapData() {
    checkBinding();
    return glUnmapBuffer(m_target);
}

uint Buffer::size() const {
    checkBinding();

    int size;
    glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &size);

    return size;
}

uint Buffer::getType() const {
    return m_target;
}

Buffer::Buffer(Object *parent, uint target)
    : ContextObject(parent), m_target(target)
{
    glGenBuffers(1, &m_id);
}
}
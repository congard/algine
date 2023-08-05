#include <algine/core/buffers/IndexBuffer.h>

AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(algine::IndexBuffer) {
    .obj = []() {
        auto buff = new algine::IndexBuffer(std::false_type {});
        buff->m_id = 0;
        buff->m_target = Buffer::Index;
        return buff;
    }()
};

algine::IndexBuffer::IndexBuffer(Object *parent)
    : Buffer(parent, Buffer::Index) {}

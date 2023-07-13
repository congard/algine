#include <algine/core/buffers/ArrayBuffer.h>

AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(algine::ArrayBuffer) {
    .obj = []() {
        auto buff = new algine::ArrayBuffer(std::false_type {});
        buff->m_id = 0;
        buff->m_target = Buffer::Array;
        return buff;
    }()
};

algine::ArrayBuffer::ArrayBuffer()
    : Buffer(Buffer::Array) {}

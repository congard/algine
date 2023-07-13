#include <algine/core/buffers/UniformBuffer.h>

#include <algine/gl.h>

namespace algine {
AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(UniformBuffer) {
    .obj = []() {
        auto buff = new UniformBuffer(std::false_type {});
        buff->m_id = 0;
        buff->m_target = Buffer::Uniform;
        return buff;
    }()
};

UniformBuffer::UniformBuffer()
    : Buffer(Buffer::Uniform) {}

uint UniformBuffer::getOffsetAlignment() {
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    return alignment;
}
}

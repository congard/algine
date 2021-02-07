#include <algine/core/buffers/UniformBuffer.h>

#include <algine/gl.h>

namespace algine {
UniformBuffer::UniformBuffer()
    : Buffer(Buffer::Uniform) {}

uint UniformBuffer::getOffsetAlignment() {
    int alignment;

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    return alignment;
}
}

#include <algine/core/buffers/UniformBuffer.h>

#include <GL/glew.h>

namespace algine {
UniformBuffer::UniformBuffer() : Buffer(Buffer::Uniform) { /* empty */ }

uint UniformBuffer::getOffsetAlignment() {
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    return alignment;
}
}

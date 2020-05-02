#include <algine/core/InputLayout.h>
#include <GL/glew.h>
#include <iostream>

using namespace std;

namespace algine {
InputLayout::InputLayout(): m_id() {
    glGenVertexArrays(1, &m_id);
}

InputLayout::~InputLayout() {
    glDeleteVertexArrays(1, &m_id);
}

void InputLayout::bind() const {
    glBindVertexArray(m_id);
}

void InputLayout::unbind() const {
    glBindVertexArray(0);
}

// TODO: type constants
void
InputLayout::addAttribute(
        const InputAttributeDescription &inputAttributeDescription,
        const ArrayBuffer *arrayBuffer) const
{
    glEnableVertexAttribArray(inputAttributeDescription.m_location);
    arrayBuffer->bind();

    switch (inputAttributeDescription.m_format) {
        case GL_FLOAT:
        case GL_HALF_FLOAT:
        case GL_DOUBLE:
            glVertexAttribPointer(
                inputAttributeDescription.m_location,
                inputAttributeDescription.m_count,
                inputAttributeDescription.m_format,
                GL_FALSE,
                inputAttributeDescription.m_stride,
                reinterpret_cast<const void *>(inputAttributeDescription.m_offset * sizeof(float))
            );
            break;
        case GL_UNSIGNED_BYTE:
        case GL_UNSIGNED_INT:
        case GL_INT:
        case GL_BYTE:
            glVertexAttribIPointer(
                inputAttributeDescription.m_location,
                inputAttributeDescription.m_count,
                inputAttributeDescription.m_format,
                inputAttributeDescription.m_stride,
                reinterpret_cast<const void *>(inputAttributeDescription.m_offset * sizeof(float))
            );
            break;
        default:
            cerr << "Unknown format " << inputAttributeDescription.m_format << "\n";
            break;
    }

    arrayBuffer->unbind();
}

void InputLayout::setIndexBuffer(const IndexBuffer *indexBuffer) const {
    indexBuffer->bind();
}
}
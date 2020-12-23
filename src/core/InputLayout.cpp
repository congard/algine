#include <algine/core/InputLayout.h>

#include <algine/core/Engine.h>

#include <GL/glew.h>
#include <iostream>

#define SOP_BOUND_PTR Engine::m_boundInputLayout
#define SOP_OBJECT_TYPE SOPConstants::InputLayoutObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::InputLayoutStr
#include "internal/SOP.h"
#include "internal/SOPConstants.h"

using namespace std;

namespace algine {
InputLayout::InputLayout(): m_id() {
    glGenVertexArrays(1, &m_id);
}

InputLayout::~InputLayout() {
    glDeleteVertexArrays(1, &m_id);
}

void InputLayout::bind() const {
    commitBinding()
    glBindVertexArray(m_id);
}

void InputLayout::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindVertexArray(0);
}

void
InputLayout::addAttribute(
        const InputAttributeDescription &inputAttribDescription,
        const ArrayBuffer *arrayBuffer) const
{
    checkBinding()

    glEnableVertexAttribArray(inputAttribDescription.m_location);
    arrayBuffer->bind();

    switch (inputAttribDescription.m_dataType) {
        case DataType::Float:
        case DataType::HalfFloat:
        case DataType::Double:
            glVertexAttribPointer(
                    inputAttribDescription.m_location,
                    inputAttribDescription.m_count,
                    static_cast<GLenum>(inputAttribDescription.m_dataType),
                    GL_FALSE,
                    inputAttribDescription.m_stride,
                    reinterpret_cast<const void *>(inputAttribDescription.m_offset * sizeof(float))
            );
            break;
        case DataType::Byte:
        case DataType::UnsignedByte:
        case DataType::Short:
        case DataType::UnsignedShort:
        case DataType::Int:
        case DataType::UnsignedInt:
            glVertexAttribIPointer(
                    inputAttribDescription.m_location,
                    inputAttribDescription.m_count,
                    static_cast<GLenum>(inputAttribDescription.m_dataType),
                    inputAttribDescription.m_stride,
                    reinterpret_cast<const void *>(inputAttribDescription.m_offset * sizeof(float))
            );
            break;
        default:
            cerr << "Unknown format " << static_cast<uint>(inputAttribDescription.m_dataType) << "\n";
            break;
    }

    arrayBuffer->unbind();
}

void InputLayout::setIndexBuffer(const IndexBuffer *indexBuffer) const {
    checkBinding()
    indexBuffer->bind();
}
}
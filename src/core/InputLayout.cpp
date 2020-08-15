#include <algine/core/InputLayout.h>
#include <algine/core/Engine.h>

#include <GL/glew.h>
#include <iostream>

#include "NativeConstants.h"

#define SOP_BOUND_PTR Engine::m_boundInputLayout
#define SOP_OBJECT_TYPE SOPConstants::InputLayoutObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::InputLayoutStr
#include "SOP.h"
#include "SOPConstants.h"

using namespace std;
using namespace algine::NativeConstants;

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
        const InputAttributeDescription &inputAttributeDescription,
        const ArrayBuffer *arrayBuffer) const
{
    checkBinding()

    glEnableVertexAttribArray(inputAttributeDescription.m_location);
    arrayBuffer->bind();

    switch (inputAttributeDescription.m_format) {
        case Types::Float:
        case Types::HalfFloat:
        case Types::Double:
            glVertexAttribPointer(
                inputAttributeDescription.m_location,
                inputAttributeDescription.m_count,
                inputAttributeDescription.m_format,
                GL_FALSE,
                inputAttributeDescription.m_stride,
                reinterpret_cast<const void *>(inputAttributeDescription.m_offset * sizeof(float))
            );
            break;
        case Types::Byte:
        case Types::UnsignedByte:
        case Types::Short:
        case Types::UnsignedShort:
        case Types::Int:
        case Types::UnsignedInt:
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
    checkBinding()
    indexBuffer->bind();
}
}
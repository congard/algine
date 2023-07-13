#include <algine/core/InputLayout.h>
#include <algine/gl.h>

#include <iostream>

using namespace std;

namespace algine {
AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(InputLayout) {
    .obj = []() {
        auto layout = new InputLayout(std::false_type {});
        layout->m_id = 0;
        return layout;
    }()
};

InputLayout::InputLayout() {
    glGenVertexArrays(1, &m_id);
}

InputLayout::~InputLayout() {
    glDeleteVertexArrays(1, &m_id);
}

void InputLayout::bind() const {
    commitBinding();
    glBindVertexArray(m_id);
}

void InputLayout::unbind() const {
    checkBinding();
    commitUnbinding();
    glBindVertexArray(0);
}

void InputLayout::addAttribute(
        const InputAttributeDescription &inputAttribDescription,
        const ArrayBuffer *arrayBuffer) const
{
    checkBinding();

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
    checkBinding();
    indexBuffer->bind();
}
}
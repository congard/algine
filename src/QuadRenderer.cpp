#include <algine/QuadRenderer.h>

namespace algine {
QuadRenderer::QuadRenderer(const uint inPosLocation) {
    const float vertices[] = {
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
    };

    m_arrayBuffer = new ArrayBuffer();
    m_arrayBuffer->bind();
    m_arrayBuffer->setData(sizeof(vertices), vertices, ArrayBuffer::StaticDraw);
    m_arrayBuffer->unbind();

    InputAttributeDescription attribDescription;
    attribDescription.setLocation(inPosLocation);
    attribDescription.setCount(3);

    m_inputLayout = new InputLayout();
    m_inputLayout->bind();
    m_inputLayout->addAttribute(attribDescription, m_arrayBuffer);
    m_inputLayout->unbind();
}

QuadRenderer::~QuadRenderer() {
    InputLayout::destroy(m_inputLayout);
    ArrayBuffer::destroy(m_arrayBuffer);
}

void QuadRenderer::draw() {
    // NOTE: add check for current CR binding
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

InputLayout* QuadRenderer::getInputLayout() const {
    return m_inputLayout;
}

ArrayBuffer* QuadRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}
}

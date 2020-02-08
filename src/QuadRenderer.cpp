#include <algine/QuadRenderer.h>

namespace algine {
QuadRenderer::QuadRenderer(const int inPosLocation, const int inTexCoordLocation) {
    const float
        vertices[12] = {
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
        },

        texCoords[8] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

    m_vertexArrayBuffer = new ArrayBuffer();
    m_vertexArrayBuffer->bind();
    m_vertexArrayBuffer->setData(sizeof(vertices), vertices, ArrayBuffer::StaticDraw);
    m_vertexArrayBuffer->unbind();

    m_texCoordArrayBuffer = new ArrayBuffer();
    m_texCoordArrayBuffer->bind();
    m_texCoordArrayBuffer->setData(sizeof(texCoords), texCoords, ArrayBuffer::StaticDraw);
    m_texCoordArrayBuffer->unbind();

    InputAttributeDescription attribDescription;
    m_inputLayout = new InputLayout();
    m_inputLayout->bind();
    attribDescription.setLocation(inPosLocation);
    attribDescription.setCount(3);
    m_inputLayout->addAttribute(attribDescription, m_vertexArrayBuffer);
    attribDescription.setLocation(inTexCoordLocation);
    attribDescription.setCount(2);
    m_inputLayout->addAttribute(attribDescription, m_texCoordArrayBuffer);
    m_inputLayout->unbind();
}

QuadRenderer::~QuadRenderer() {
    InputLayout::destroy(m_inputLayout);
    ArrayBuffer::destroy(m_vertexArrayBuffer, m_texCoordArrayBuffer);
}

void QuadRenderer::draw() {
    // NOTE: add check for current CR binding
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

InputLayout* QuadRenderer::getInputLayout() const {
    return m_inputLayout;
}
}

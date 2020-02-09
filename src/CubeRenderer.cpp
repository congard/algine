#include <algine/CubeRenderer.h>

namespace algine {
CubeRenderer::CubeRenderer(const uint inPosLocation) {
    // source: https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
    const float vertices[] = {
            -1.0f,  1.0f,  1.0f,    // Front-top-left
             1.0f,  1.0f,  1.0f,    // Front-top-right
            -1.0f, -1.0f,  1.0f,    // Front-bottom-left
             1.0f, -1.0f,  1.0f,    // Front-bottom-right
             1.0f, -1.0f, -1.0f,    // Back-bottom-right
             1.0f,  1.0f,  1.0f,    // Front-top-right
             1.0f,  1.0f, -1.0f,    // Back-top-right
            -1.0f,  1.0f,  1.0f,    // Front-top-left
            -1.0f,  1.0f, -1.0f,    // Back-top-left
            -1.0f, -1.0f,  1.0f,    // Front-bottom-left
            -1.0f, -1.0f, -1.0f,    // Back-bottom-left
             1.0f, -1.0f, -1.0f,    // Back-bottom-right
            -1.0f,  1.0f, -1.0f,    // Back-top-left
             1.0f,  1.0f, -1.0f     // Back-top-right
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

CubeRenderer::~CubeRenderer() {
    InputLayout::destroy(m_inputLayout);
    ArrayBuffer::destroy(m_arrayBuffer);
}

void CubeRenderer::draw() {
    // NOTE: add check for current CR binded
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
}

InputLayout* CubeRenderer::getInputLayout() const {
    return m_inputLayout;
}

ArrayBuffer* CubeRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}
}
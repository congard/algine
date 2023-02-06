#include <algine/std/CubeRenderer.h>

#include <algine/gl.h>

namespace algine {
CubeRenderer::CubeRenderer(uint inPosLocation)
    : m_arrayBuffer(std::make_unique<ArrayBuffer>()),
      m_inputLayout(std::make_unique<InputLayout>())
{
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

    m_arrayBuffer->bind();
    m_arrayBuffer->setData(sizeof(vertices), vertices, ArrayBuffer::StaticDraw);
    m_arrayBuffer->unbind();

    InputAttributeDescription attribDescription;
    attribDescription.setLocation(inPosLocation);
    attribDescription.setCount(3);

    m_inputLayout->bind();
    m_inputLayout->addAttribute(attribDescription, m_arrayBuffer.get());
    m_inputLayout->unbind();
}

void CubeRenderer::draw() {
    // NOTE: add check if current CR bound
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
}

const std::unique_ptr<InputLayout>& CubeRenderer::getInputLayout() const {
    return m_inputLayout;
}

const std::unique_ptr<ArrayBuffer>& CubeRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}
}
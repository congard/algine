#include <algine/std/QuadRenderer.h>

#include <algine/core/shader/ShaderCreator.h>

#include <algine/gl.h>

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
    delete m_inputLayout;
    delete m_arrayBuffer;
}

void QuadRenderer::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

InputLayout* QuadRenderer::getInputLayout() const {
    return m_inputLayout;
}

ArrayBuffer* QuadRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}

ShaderPtr QuadRenderer::getVertexShader() {
    constexpr auto name = "Quad.vs";

    if (const ShaderPtr &ptr = Shader::getByName(name); ptr != nullptr) {
        return ptr;
    } else {
        ShaderCreator creator(Shader::Vertex);
        creator.setAccess(Creator::Access::Public);
        creator.setName(name);
        creator.setPath("@algine/Quad.vs.glsl");
        return creator.create();
    }
}
}

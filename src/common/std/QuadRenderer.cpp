#include <algine/std/QuadRenderer.h>

#include <algine/core/shader/ShaderCreator.h>

#include <algine/gl.h>

namespace algine {
QuadRenderer::QuadRenderer(uint inPosLocation)
    : m_arrayBuffer(std::make_unique<ArrayBuffer>()),
      m_inputLayout(std::make_unique<InputLayout>())
{
    const float vertices[] = {
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
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

void QuadRenderer::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

const std::unique_ptr<InputLayout>& QuadRenderer::getInputLayout() const {
    return m_inputLayout;
}

const std::unique_ptr<ArrayBuffer>& QuadRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}

ShaderPtr QuadRenderer::getVertexShader() {
    constexpr auto name = "Quad.vs";

    if (const ShaderPtr &ptr = Shader::getByName(name); ptr != nullptr) {
        return ptr;
    } else {
        ShaderCreator creator(Shader::Type::Vertex);
        creator.setAccess(Creator::Access::Public);
        creator.setName(name);
        creator.setPath("@algine/Quad.vs.glsl");
        return creator.create();
    }
}
}

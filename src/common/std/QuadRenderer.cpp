#include <algine/std/QuadRenderer.h>
#include <algine/core/shader/ShaderBuilder.h>
#include <algine/core/scene/GlobalScene.h>

#include <algine/gl.h>

namespace algine {
QuadRenderer::QuadRenderer(uint inPosLocation, Object *parent)
    : m_arrayBuffer(new ArrayBuffer(parent)),
      m_inputLayout(new InputLayout(parent))
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
    m_inputLayout->addAttribute(attribDescription, m_arrayBuffer);
    m_inputLayout->unbind();
}

QuadRenderer::QuadRenderer(Object *parent)
    : QuadRenderer(0, parent) {}

void QuadRenderer::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

InputLayout* QuadRenderer::getInputLayout() const {
    return m_inputLayout;
}

ArrayBuffer* QuadRenderer::getArrayBuffer() const {
    return m_arrayBuffer;
}

Shader* QuadRenderer::getVertexShader() {
    constexpr auto name = "Quad.vs";

    // We need this mutex to prevent the simultaneous loading of
    // vertex shaders, which can lead to a runtime error. If two
    // objects with the same name are added to the global scene
    // as children, it can cause issues
    static std::mutex mutex;
    std::lock_guard locker(mutex);

    if (auto ptr = GlobalScene::getInstance()->findChild<Shader*>(name, Object::FindOption::Direct); ptr) {
        return ptr;
    } else {
        ShaderBuilder builder(Shader::Type::Vertex);
        builder.setParent(GlobalScene::getInstance());
        builder.setName(name);
        builder.setPath("@algine/Quad.vs.glsl");
        return builder.create();
    }
}
}

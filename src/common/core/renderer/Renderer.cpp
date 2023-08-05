#include <algine/core/renderer/Renderer.h>
#include <algine/core/renderer/RenderPass.h>

namespace algine {
Renderer::Renderer(Scene *parent)
    : Object(parent) {}

void Renderer::setScene(Scene *parent) {
    setParent(parent);
}

void Renderer::setParent(Object *parent) {
    if (dynamic_cast<Scene*>(parent) == nullptr)
        throw std::runtime_error("Unexpected type: only Scene instances are allowed");
    Object::setParent(parent);
}

void Renderer::addPass(RenderPass *pass) {
    pass->setParent(this);
    m_passes.emplace_back(pass);
}

void Renderer::insertPass(RenderPass *pass, Index index) {
    pass->setParent(this);
    m_passes.insert(m_passes.begin() + index, pass);
}

bool Renderer::removePass(RenderPass *pass, bool autoDelete) {
    assert(pass->getParent() == this);

    if (auto it = std::find(m_passes.begin(), m_passes.end(), pass); it != m_passes.end()) {
        m_passes.erase(it);

        if (autoDelete)
            delete pass;

        return true;
    }

    return false;
}

const std::vector<RenderPass*>& Renderer::getPasses() const {
    return m_passes;
}

void Renderer::render() {
    for (auto pass : m_passes) {
        pass->render();
    }
}
} // algine
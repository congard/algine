#include <algine/core/renderer/RenderPass.h>
#include <algine/core/renderer/Renderer.h>

namespace algine {
RenderPass::RenderPass(Renderer *parent)
    : Object(parent) {}

void RenderPass::setParent(Object *parent) {
    if (dynamic_cast<Renderer*>(parent) == nullptr)
        throw std::runtime_error("Unexpected type: only Renderer instances are allowed");
    Object::setParent(parent);
}
} // algine
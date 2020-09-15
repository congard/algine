#include <algine/core/RenderbufferManager.h>

#include <stdexcept>

#include "PublicObjectTools.h"

using namespace std;

namespace algine {
RenderbufferPtr RenderbufferManager::getRenderbuffer() {
    return PublicObjectTools::getPtr(this, &RenderbufferManager::createRenderbuffer);
}

RenderbufferPtr RenderbufferManager::createRenderbuffer() {
    RenderbufferPtr renderbuffer = make_shared<Renderbuffer>();
    renderbuffer->setName(m_name);
    renderbuffer->setFormat(m_format);
    renderbuffer->setDimensions(m_width, m_height);

    renderbuffer->bind();
    renderbuffer->update();
    renderbuffer->unbind();

    if (m_access == Access::Public) {
        if (m_name.empty())
            throw runtime_error("Renderbuffer without name can't be public");

        if (Renderbuffer::byName(m_name) == nullptr) {
            Renderbuffer::publicRenderbuffers.emplace_back(renderbuffer);
        } else {
            throw runtime_error("Renderbuffer with the same name was already loaded");
        }
    }

    return renderbuffer;
}
}

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

    PublicObjectTools::postCreateAccessOp("Renderbuffer", this, renderbuffer);

    return renderbuffer;
}
}

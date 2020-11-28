#include <algine/core/RenderbufferManager.h>

#include <algine/core/PtrMaker.h>

#include "PublicObjectTools.h"

namespace algine {
RenderbufferPtr RenderbufferManager::get() {
    return PublicObjectTools::getPtr<RenderbufferPtr>(this);
}

RenderbufferPtr RenderbufferManager::create() {
    RenderbufferPtr renderbuffer = PtrMaker::make();
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

#include <algine/core/RenderbufferCreator.h>
#include <algine/core/Renderbuffer.h>
#include <algine/core/PtrMaker.h>

#include "internal/PublicObjectTools.h"

using namespace algine::internal;

namespace algine {
RenderbufferPtr RenderbufferCreator::get() {
    return PublicObjectTools::getPtr<RenderbufferPtr>(this);
}

RenderbufferPtr RenderbufferCreator::create() {
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

void RenderbufferCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<RenderbufferCreator>(s, path, lua, tenv);
}
}

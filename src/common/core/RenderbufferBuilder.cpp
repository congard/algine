#include <algine/core/RenderbufferBuilder.h>
#include <algine/core/Renderbuffer.h>

namespace algine {
Object* RenderbufferBuilder::createImpl() {
    auto renderbuffer = new Renderbuffer(getActualParent());
    renderbuffer->setName(m_name);
    renderbuffer->setFormat(m_format);
    renderbuffer->setDimensions(m_width, m_height);

    renderbuffer->bind();
    renderbuffer->update();
    renderbuffer->unbind();

    return renderbuffer;
}

void RenderbufferBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<RenderbufferBuilder>(s, path, lua, tenv);
}
}

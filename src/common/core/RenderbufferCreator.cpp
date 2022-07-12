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

void RenderbufferCreator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "RenderbufferCreator"))
        return;

    lua->registerUsertype<ImageCreator, Renderbuffer>();

    auto ctors = sol::constructors<RenderbufferCreator()>();
    auto usertype = lua->state()->new_usertype<RenderbufferCreator>(
            "RenderbufferCreator",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator, ImageCreator>());

    usertype["get"] = &RenderbufferCreator::get;
    usertype["create"] = &RenderbufferCreator::create;
}

void RenderbufferCreator::exec(const std::string &s, bool path, Lua *lua) {
    exec_t<RenderbufferCreator>(s, path, lua);
}
}

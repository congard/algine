#include <algine/core/texture/Texture2DCreator.h>
#include <algine/core/texture/Texture2D.h>

#include <tulz/Path.h>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace algine::internal;

namespace algine {
Texture2DCreator::Texture2DCreator() {
    m_type = TextureCreator::Type::Texture2D;
    m_defaultParams = Texture2D::defaultParams();
}

void Texture2DCreator::setPath(std::string_view path) {
    m_path = path;
}

const std::string& Texture2DCreator::getPath() const {
    return m_path;
}

Texture2DPtr Texture2DCreator::get() {
    return PublicObjectTools::getPtr<Texture2DPtr>(this);
}

Texture2DPtr Texture2DCreator::create() {
    if (m_type != TextureCreator::Type::Texture2D)
        throw runtime_error("Invalid texture type. Use a different creator");

    Texture2DPtr texture = make_shared<Texture2D>();
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_path.empty()) {
        TextureFileInfo info;
        info.path = Path::join(m_workingDirectory, m_path);
        info.dataType = m_dataType;
        info.flip = true;
        info.ioSystem = io();
        texture->fromFile(info);
    } else {
        texture->setDimensions(m_width, m_height);
        texture->update();
    }

    texture->setParams(m_params.empty() ? m_defaultParams : m_params);

    texture->unbind();

    PublicObjectTools::postCreateAccessOp("Texture2D", this, texture);

    return texture;
}

void Texture2DCreator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Texture2DCreator"))
        return;

    lua->registerUsertype<TextureCreator, Texture2D>();

    auto ctors = sol::constructors<Texture2DCreator()>();
    auto usertype = lua->state()->new_usertype<Texture2DCreator>(
            "Texture2DCreator",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator, ImageCreator, TextureCreator>());

    usertype["get"] = &Texture2DCreator::get;
    usertype["create"] = &Texture2DCreator::create;

    Lua::new_property(usertype, "path", "getPath", "setPath", &Texture2DCreator::getPath, &Texture2DCreator::setPath);
}

void Texture2DCreator::exec(const std::string &s, bool path, Lua *lua) {
    exec_t<Texture2DCreator>(s, path, lua);
}
}

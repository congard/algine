#include <algine/core/texture/TextureCubeCreator.h>

#include <tulz/Path.h>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace algine::internal;

namespace algine {
TextureCubeCreator::TextureCubeCreator() {
    m_type = TextureCreator::Type::TextureCube;
    m_defaultParams = TextureCube::defaultParams();
}

void TextureCubeCreator::setPath(std::string_view path, TextureCube::Face face) {
    m_paths[face] = path;
}

const std::string& TextureCubeCreator::getPath(TextureCube::Face face) const {
    return m_paths.at(face);
}

void TextureCubeCreator::setPaths(const map<TextureCube::Face, string> &paths) {
    m_paths = paths;
}

const map<TextureCube::Face, string>& TextureCubeCreator::getPaths() const {
    return m_paths;
}

TextureCubePtr TextureCubeCreator::get() {
    return PublicObjectTools::getPtr<TextureCubePtr>(this);
}

TextureCubePtr TextureCubeCreator::create() {
    if (m_type != TextureCreator::Type::TextureCube)
        throw runtime_error("Invalid texture type. Use a different creator");

    TextureCubePtr texture = make_shared<TextureCube>();
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_paths.empty()) {
        for (const auto &path : m_paths) {
            TextureFileInfo info;
            info.path = Path::join(m_workingDirectory, path.second);
            info.dataType = m_dataType;
            info.ioSystem = io();
            texture->fromFile(path.first, info);
        }
    } else {
        texture->setDimensions(m_width, m_height);
        texture->update();
    }

    texture->setParams(m_params.empty() ? m_defaultParams : m_params);

    texture->unbind();

    PublicObjectTools::postCreateAccessOp("TextureCube", this, texture);

    return texture;
}

void TextureCubeCreator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "TextureCubeCreator"))
        return;

    lua->registerUsertype<TextureCreator, TextureCube>();

    auto ctors = sol::constructors<TextureCubeCreator()>();
    auto usertype = lua->state()->new_usertype<TextureCubeCreator>(
            "TextureCubeCreator",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator, ImageCreator, TextureCreator>());

    usertype["get"] = &TextureCubeCreator::get;
    usertype["create"] = &TextureCubeCreator::create;
    usertype["setPath"] = &TextureCubeCreator::setPath;
    usertype["getPath"] = &TextureCubeCreator::getPath;

    Lua::new_property(usertype, "paths", &TextureCubeCreator::getPaths,
        [](TextureCubeCreator &self, std::map<TextureCube::Face, std::string> paths) { self.setPaths(paths); });
}

void TextureCubeCreator::exec(const std::string &s, bool path, Lua *lua) {
    exec_t<TextureCubeCreator>(s, path, lua);
}
}

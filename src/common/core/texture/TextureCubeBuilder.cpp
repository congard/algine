#include <algine/core/texture/TextureCubeBuilder.h>

#include <tulz/Path.h>

using namespace std;
using namespace tulz;

namespace algine {
TextureCubeBuilder::TextureCubeBuilder() {
    m_type = TextureBuilder::Type::TextureCube;
    m_defaultParams = TextureCube::defaultParams();
}

void TextureCubeBuilder::setPath(std::string_view path, TextureCube::Face face) {
    m_paths[face] = path;
}

const std::string& TextureCubeBuilder::getPath(TextureCube::Face face) const {
    return m_paths.at(face);
}

void TextureCubeBuilder::setPaths(const map<TextureCube::Face, string> &paths) {
    m_paths = paths;
}

const map<TextureCube::Face, string>& TextureCubeBuilder::getPaths() const {
    return m_paths;
}

Object* TextureCubeBuilder::createImpl() {
    if (m_type != TextureBuilder::Type::TextureCube)
        throw runtime_error("Invalid texture type. Use a different builder");

    auto texture = new TextureCube(getActualParent());
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_paths.empty()) {
        for (const auto &path : m_paths) {
            TextureFileInfo info;
            info.path = Path::join(getRootDir(), path.second);
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

    return texture;
}

void TextureCubeBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<TextureCubeBuilder>(s, path, lua, tenv);
}
}

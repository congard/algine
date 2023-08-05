#include <algine/core/texture/Texture2DBuilder.h>
#include <algine/core/texture/Texture2D.h>

#include <tulz/Path.h>

using namespace std;
using namespace tulz;

namespace algine {
Texture2DBuilder::Texture2DBuilder() {
    m_type = TextureBuilder::Type::Texture2D;
    m_defaultParams = Texture2D::defaultParams();
}

void Texture2DBuilder::setPath(std::string_view path) {
    m_path = path;
}

const std::string& Texture2DBuilder::getPath() const {
    return m_path;
}

Object* Texture2DBuilder::createImpl() {
    if (m_type != TextureBuilder::Type::Texture2D)
        throw runtime_error("Invalid texture type. Use a different builder");

    auto texture = new Texture2D(getActualParent());
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_path.empty()) {
        TextureFileInfo info;
        info.path = Path::join(getRootDir(), m_path);
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

    return texture;
}

void Texture2DBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<Texture2DBuilder>(s, path, lua, tenv);
}
}

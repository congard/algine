#include <algine/core/texture/Texture2DCreator.h>

#include <algine/core/texture/Texture2D.h>
#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>

#include "internal/ConfigStrings.h"
#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace nlohmann;
using namespace algine::internal;

namespace algine {
Texture2DCreator::Texture2DCreator() {
    m_type = TextureCreator::Type::Texture2D;
    m_defaultParams = Texture2D::defaultParams();
}

void Texture2DCreator::setPath(const string &path) {
    m_path = path;
}

std::string Texture2DCreator::getPath() const {
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

void Texture2DCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    if (config.contains(File) && config[File].contains(Path))
        m_path = config[File][Path];

    TextureCreator::import(jsonHelper);
}

JsonHelper Texture2DCreator::dump() {
    JsonHelper config;

    if (!m_path.empty())
        config.json[Config::File][Config::Path] = m_path;

    m_writeFileSection = !m_path.empty();

    config.append(TextureCreator::dump());

    return config;
}
}

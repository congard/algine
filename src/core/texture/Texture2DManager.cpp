#include <algine/core/texture/Texture2DManager.h>

#include <algine/core/texture/Texture2D.h>

#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(File, "file");
constant(Path, "path");
}

namespace algine {
Texture2DManager::Texture2DManager() {
    m_type = TextureManager::Type::Texture2D;
    m_defaultParams = Texture2D::defaultParams();
}

void Texture2DManager::setPath(const string &path) {
    m_path = path;
}

std::string Texture2DManager::getPath() const {
    return m_path;
}

Texture2DPtr Texture2DManager::createTexture() {
    if (m_type != TextureManager::Type::Texture2D)
        throw runtime_error("Invalid texture type. Use a different manager");

    Texture2DPtr texture = make_shared<Texture2D>();
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_path.empty()) {
        texture->fromFile(m_path, m_dataType);
    } else {
        texture->setDimensions(m_width, m_height);
        texture->update();
    }

    texture->setParams(m_params);

    texture->unbind();

    if (m_access == Access::Public) {
        if (m_name.empty())
            throw runtime_error("Texture2D without name can't be public");

        if (Texture2D::byName(m_name) == nullptr) {
            Texture2D::publicTextures.emplace_back(texture);
        } else {
            throw runtime_error("Texture2D with the same name was already loaded");
        }
    }

    return texture;
}

void Texture2DManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    if (config.contains(File) && config[File].contains(Path))
        m_path = config[File][Path];

    TextureManager::import(jsonHelper);
}

JsonHelper Texture2DManager::dump() {
    JsonHelper config;

    if (!m_path.empty())
        config.json[Config::File][Config::Path] = m_path;

    m_writeFileSection = !m_path.empty();

    config.append(TextureManager::dump());

    return config;
}
}

#include <algine/core/texture/TextureCubeManager.h>

#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>

#include "../ConfigStrings.h"
#include "../PublicObjectTools.h"

using namespace std;
using namespace nlohmann;
using namespace tulz;

namespace algine {
namespace Config {
constant(Right, "right");
constant(Left, "left");
constant(Top, "top");
constant(Bottom, "bottom");
constant(Back, "back");
constant(Front, "front");
}

TextureCubeManager::TextureCubeManager() {
    m_type = TextureManager::Type::TextureCube;
    m_defaultParams = TextureCube::defaultParams();
}

void TextureCubeManager::setPath(const string &path, TextureCube::Face face) {
    m_paths[face] = path;
}

string TextureCubeManager::getPath(TextureCube::Face face) const {
    return m_paths.at(face);
}

void TextureCubeManager::setPaths(const map<TextureCube::Face, string> &paths) {
    m_paths = paths;
}

const map<TextureCube::Face, string>& TextureCubeManager::getPaths() const {
    return m_paths;
}

TextureCubePtr TextureCubeManager::get() {
    return PublicObjectTools::getPtr<TextureCubePtr>(this);
}

TextureCubePtr TextureCubeManager::create() {
    if (m_type != TextureManager::Type::TextureCube)
        throw runtime_error("Invalid texture type. Use a different manager");

    TextureCubePtr texture = make_shared<TextureCube>();
    texture->setName(m_name);
    texture->setFormat(m_format);

    texture->bind();

    if (!m_paths.empty()) {
        for (const auto & path : m_paths) {
            texture->fromFile(Path::join(m_workingDirectory, path.second), path.first, m_dataType);
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

#define face_str(name) if (face == TextureCube::Face::name) return Config::name;

inline string faceToString(TextureCube::Face face) {
    face_str(Right)
    face_str(Left)
    face_str(Top)
    face_str(Bottom)
    face_str(Back)
    face_str(Front)

    throw runtime_error("Unsupported face value " + to_string(static_cast<uint>(face)));
}

#define face_type(name) if (str == Config::name) return TextureCube::Face::name;

inline TextureCube::Face stringToFace(const string &str) {
    face_type(Right)
    face_type(Left)
    face_type(Top)
    face_type(Bottom)
    face_type(Back)
    face_type(Front)

    throw runtime_error("Unsupported face value '" + str + "'");
}

void TextureCubeManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    if (config.contains(File) && config[File].contains(Paths)) {
        for (const auto &path : config[File][Paths].items()) {
            m_paths[stringToFace(path.key())] = path.value();
        }
    }

    TextureManager::import(jsonHelper);
}

JsonHelper TextureCubeManager::dump() {
    JsonHelper config;

    for (const auto & path : m_paths)
        config.json[Config::File][Config::Paths][faceToString(path.first)] = path.second;

    m_writeFileSection = !m_paths.empty();

    config.append(TextureManager::dump());

    return config;
}
}

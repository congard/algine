#include <algine/core/texture/TextureCubeCreator.h>

#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>

#include "internal/ConfigStrings.h"
#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace nlohmann;
using namespace algine::internal;

namespace algine {
namespace Config {
constant(Right, "right");
constant(Left, "left");
constant(Top, "top");
constant(Bottom, "bottom");
constant(Back, "back");
constant(Front, "front");
}

TextureCubeCreator::TextureCubeCreator() {
    m_type = TextureCreator::Type::TextureCube;
    m_defaultParams = TextureCube::defaultParams();
}

void TextureCubeCreator::setPath(const string &path, TextureCube::Face face) {
    m_paths[face] = path;
}

string TextureCubeCreator::getPath(TextureCube::Face face) const {
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

void TextureCubeCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    if (config.contains(File) && config[File].contains(Paths)) {
        for (const auto &path : config[File][Paths].items()) {
            m_paths[stringToFace(path.key())] = path.value();
        }
    }

    TextureCreator::import(jsonHelper);
}

JsonHelper TextureCubeCreator::dump() {
    JsonHelper config;

    for (const auto & path : m_paths)
        config.json[Config::File][Config::Paths][faceToString(path.first)] = path.second;

    m_writeFileSection = !m_paths.empty();

    config.append(TextureCreator::dump());

    return config;
}
}

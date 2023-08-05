#include <algine/std/Material.h>

using namespace std;

namespace algine {
void Material::setName(const std::string &name) {
    m_materialName = name;
}

const std::string& Material::getName() const {
    return m_materialName;
}

void Material::setFloat(const std::string &name, float value) {
    m_floats[name] = value;
}

void Material::setTexture2D(const std::string &name, Texture2D *value) {
    m_textures2D[name] = value;
}

float Material::getFloat(const std::string &name) const {
    return m_floats.at(name);
}

float Material::getFloat(const std::string &name, float defaultValue) const {
    if (auto it = m_floats.find(name); it != m_floats.end()) {
        return it->second;
    } else {
        return defaultValue;
    }
}

Texture2D* Material::getTexture2D(const std::string &name) const {
    return m_textures2D.at(name);
}

Texture2D* Material::getTexture2D(const std::string &name, Texture2D *defaultValue) const {
    if (auto it = m_textures2D.find(name); it != m_textures2D.end()) {
        return it->second;
    } else {
        return defaultValue;
    }
}

void Material::setFloats(const std::unordered_map<std::string, float> &floats) {
    m_floats = floats;
}

void Material::setTextures2D(const std::unordered_map<std::string, Texture2D*> &textures) {
    m_textures2D = textures;
}

const std::unordered_map<std::string, float>& Material::getFloats() const {
    return m_floats;
}

const std::unordered_map<std::string, Texture2D*>& Material::getTextures2D() const {
    return m_textures2D;
}

bool Material::hasFloat(const std::string &name) const {
    return m_floats.find(name) != m_floats.end();
}

bool Material::hasTexture2D(const std::string &name) const {
    return m_textures2D.find(name) != m_textures2D.end();
}
}

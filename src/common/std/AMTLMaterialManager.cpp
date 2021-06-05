#include <algine/std/AMTLMaterialManager.h>

#include <algine/core/JsonHelper.h>
#include <algine/core/texture/Texture2D.h>

#include <stdexcept>
#include <utility>

#include "internal/ConfigStrings.h"

using namespace std;
using namespace nlohmann;

namespace algine {
namespace Config {
constexpr auto Floats = "floats";
}

AMTLMaterialManager::AMTLMaterialManager() = default;

AMTLMaterialManager::AMTLMaterialManager(string name)
    : m_name(std::move(name)) {}

void AMTLMaterialManager::setName(const string &name) {
    m_name = name;
}

void AMTLMaterialManager::setFloat(const string &name, float value) {
    m_floats[name] = value;
}

void AMTLMaterialManager::setTexture(const string &name, const Texture2DCreator &texture) {
    m_textures[name] = texture;
}

void AMTLMaterialManager::setTextureName(const string &name, const string &texName) {
    m_texNames[name] = name;
}

void AMTLMaterialManager::setTexturePath(const string &name, const string &path) {
    m_texPaths[name] = path;
}

const string& AMTLMaterialManager::getName() const {
    return m_name;
}

float AMTLMaterialManager::getFloat(const string &name) const {
    return m_floats.at(name);
}

bool AMTLMaterialManager::hasFloat(const string &name) const {
    return m_floats.find(name) != m_floats.end();
}

bool AMTLMaterialManager::hasTexture(const string &name) const {
    auto hasTex = [&](const auto &m) {
        return m.find(name) != m.end();
    };

    return hasTex(m_textures) || hasTex(m_texPaths) || hasTex(m_texNames);
}

const unordered_map<string, float>& AMTLMaterialManager::getFloats() const {
    return m_floats;
}

const unordered_map<string, Texture2DCreator>& AMTLMaterialManager::getTextures() const {
    return m_textures;
}

const unordered_map<string, string>& AMTLMaterialManager::getTextureNames() const {
    return m_texNames;
}

const unordered_map<string, string>& AMTLMaterialManager::getTexturePaths() const {
    return m_texPaths;
}

set<string> AMTLMaterialManager::collectTextureNames() const {
    set<string> names;

    auto collect = [&](const auto &m) {
        for (const auto &p : m) {
            names.insert(p.first);
        }
    };

    collect(m_textures);
    collect(m_texNames);
    collect(m_texPaths);

    return names;
}

Texture2DPtr AMTLMaterialManager::loadTexture(const string &texName) {
    if (auto it = m_textures.find(texName); it != m_textures.end()) {
        it->second.setIOSystem(io());
        return it->second.get();
    }

    if (auto it = m_texPaths.find(texName); it != m_texPaths.end()) {
        Texture2DCreator manager;
        manager.setIOSystem(io());
        manager.setWorkingDirectory(m_workingDirectory);
        manager.importFromFile(it->second);
        return manager.get();
    }

    if (auto it = m_texNames.find(texName); it != m_texNames.end()) {
        return Texture2D::getByName(it->second);
    }

    return nullptr;
}

void AMTLMaterialManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load name
    m_name = jsonHelper.readValue<string>(Name);

    // load float values
    if (config.contains(Floats)) {
        const json &floats = config[Floats];

        for (const auto &p : floats.items()) {
            m_floats[p.key()] = p.value();
        }
    }

    // load textures
    if (config.contains(Textures)) {
        const json &textures = config[Textures];

        for (const auto &texture : textures.items()) {
            const string &texName = texture.key();

            auto isTypeLoaded = [&](const auto &m) {
                return m.find(texName) != m.end();
            };

            if (isTypeLoaded(m_textures) || isTypeLoaded(m_texPaths) || isTypeLoaded(m_texNames))
                throw runtime_error("Texture with the same name already exists. Name: " + texName);

            const json &block = texture.value();

            if (block.contains(Dump)) {
                Texture2DCreator manager;
                manager.setWorkingDirectory(m_workingDirectory);
                manager.import(block[Dump]);
                m_textures[texName] = manager;
            } else if (block.contains(Path)) {
                m_texPaths[texName] = block[Path];
            } else if (block.contains(Name)) {
                m_texNames[texName] = block[Name];
            } else {
                throw invalid_argument("Unknown Texture source '" + texture.key() + "'");
            }
        }
    }
}

JsonHelper AMTLMaterialManager::dump() {
    using namespace Config;

    json config;

    // write name
    if (!m_name.empty())
        config[Name] = m_name;

    // write float values
    json &floats = config[Floats];

    for (auto &p : m_floats)
        floats[p.first] = p.second;

    // write textures
    json &textures = config[Textures];

    for (auto &texture : m_textures)
        textures[texture.first][Dump] = texture.second.dump().json;

    for (auto &name : m_texNames)
        textures[name.first][Name] = name.second;

    for (auto &path : m_texPaths)
        textures[path.first][Path] = path.second;

    return config;
}
}

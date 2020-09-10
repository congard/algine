#include <algine/std/AMTLMaterialManager.h>

#include <algine/core/JsonHelper.h>
#include <algine/core/texture/Texture2D.h>

#include <tulz/Path.h>

#include <stdexcept>

#include "../core/ConfigStrings.h"

using namespace std;
using namespace nlohmann;
using namespace tulz;

namespace algine {
namespace Config {
constant(AmbientStrength, "ambientStrength");
constant(DiffuseStrength, "diffuseStrength");
constant(SpecularStrength, "specularStrength");
constant(Shininess, "shininess");

constant(Ambient, "ambient");
constant(Diffuse, "diffuse");
constant(Specular, "specular");
constant(Normal, "normal");
constant(Reflection, "reflection");
constant(Jitter, "jitter");
}

namespace Default {
constexpr float AmbientStrength = 0.01f;
constexpr float DiffuseStrength = 1.0f;
constexpr float SpecularStrength = 1.0f;
constexpr float Reflection = 0.5f;
constexpr float Jitter = 0.25f;
constexpr float Shininess = -1.0f;
}

AMTLMaterialManager::AMTLMaterialManager()
    : m_ambientStrength(Default::AmbientStrength),
      m_diffuseStrength(Default::DiffuseStrength),
      m_specularStrength(Default::SpecularStrength),
      m_reflection(Default::Reflection),
      m_jitter(Default::Jitter),
      m_shininess(Default::Shininess)
{
    // see initializer list above
}

AMTLMaterialManager::AMTLMaterialManager(const std::string &name)
    : AMTLMaterialManager()
{
    setName(name);
}

void AMTLMaterialManager::setName(const string &name) {
    m_name = name;
}

void AMTLMaterialManager::setAmbientStrength(float ambientStrength) {
    m_ambientStrength = ambientStrength;
}

void AMTLMaterialManager::setDiffuseStrength(float diffuseStrength) {
    m_diffuseStrength = diffuseStrength;
}

void AMTLMaterialManager::setSpecularStrength(float specularStrength) {
    m_specularStrength = specularStrength;
}

void AMTLMaterialManager::setShininess(float shininess) {
    m_shininess = shininess;
}

void AMTLMaterialManager::setReflection(float reflection) {
    m_reflection = reflection;
}

void AMTLMaterialManager::setJitter(float jitter) {
    m_jitter = jitter;
}

void AMTLMaterialManager::setTextures(const map<Texture, Texture2DManager> &textures) {
    m_textures = textures;
}

void AMTLMaterialManager::setTextureNames(const map<Texture, string> &names) {
    m_texNames = names;
}

void AMTLMaterialManager::setTexturePaths(const map<Texture, string> &paths) {
    m_texPaths = paths;
}

void AMTLMaterialManager::setTexture(Texture type, const Texture2DManager &texture) {
    m_textures[type] = texture;
}

void AMTLMaterialManager::setTextureName(Texture type, const string &name) {
    m_texNames[type] = name;
}

void AMTLMaterialManager::setTexturePath(Texture type, const string &path) {
    m_texPaths[type] = path;
}

const string& AMTLMaterialManager::getName() const {
    return m_name;
}

float AMTLMaterialManager::getAmbientStrength() const {
    return m_ambientStrength;
}

float AMTLMaterialManager::getDiffuseStrength() const {
    return m_diffuseStrength;
}

float AMTLMaterialManager::getSpecularStrength() const {
    return m_specularStrength;
}

float AMTLMaterialManager::getShininess() const {
    return m_shininess;
}

float AMTLMaterialManager::getReflection() const {
    return m_reflection;
}

float AMTLMaterialManager::getJitter() const {
    return m_jitter;
}

const map<AMTLMaterialManager::Texture, Texture2DManager>& AMTLMaterialManager::getTextures() const {
    return m_textures;
}

const map<AMTLMaterialManager::Texture, string>& AMTLMaterialManager::getTextureNames() const {
    return m_texNames;
}

const map<AMTLMaterialManager::Texture, string>& AMTLMaterialManager::getTexturePaths() const {
    return m_texPaths;
}

Texture2DPtr AMTLMaterialManager::loadTexture(Texture type) {
    // if (auto it = m_textures.find(type); it != m_textures.end()) { ... }
    // supported only in C++17

    {
        auto it = m_textures.find(type);

        if (it != m_textures.end()) {
            return it->second.getTexture();
        }
    }

    {
        auto it = m_texPaths.find(type);

        if (it != m_texPaths.end()) {
            Texture2DManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.importFromFile(it->second);
            return manager.getTexture();
        }
    }

    {
        auto it = m_texNames.find(type);

        if (it != m_texNames.end()) {
            return Texture2D::getByName(it->second);
        }
    }

    return nullptr;
}

#define textureType_str(element) if (type == AMTLMaterialManager::Texture::element) return Config::element;

inline string textureTypeToString(AMTLMaterialManager::Texture type) {
    textureType_str(Ambient)
    textureType_str(Diffuse)
    textureType_str(Specular)
    textureType_str(Normal)
    textureType_str(Reflection)
    textureType_str(Jitter)

    throw runtime_error("Unknown texture type " + to_string(static_cast<uint>(type)));
}

#define textureType_type(element) if (str == Config::element) return AMTLMaterialManager::Texture::element;

inline AMTLMaterialManager::Texture stringToTextureType(const string &str) {
    textureType_type(Ambient)
    textureType_type(Diffuse)
    textureType_type(Specular)
    textureType_type(Normal)
    textureType_type(Reflection)
    textureType_type(Jitter)

    throw runtime_error("Unknown texture type '" + str + "'");
}

void AMTLMaterialManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    // load name
    m_name = jsonHelper.readValue<string>(Name);

    // load properties
#define loadProp(name) jsonHelper.readValue<float>(name, Default::name)

    m_ambientStrength = loadProp(AmbientStrength);
    m_diffuseStrength = loadProp(DiffuseStrength);
    m_specularStrength = loadProp(SpecularStrength);
    m_reflection = loadProp(Reflection);
    m_jitter = loadProp(Jitter);
    m_shininess = loadProp(Shininess);

#undef loadProp

    // load textures
    const json &textures = jsonHelper.json[Textures];

    for (const auto & texture : textures.items()) {
        Texture type = stringToTextureType(texture.key());

        auto isTypeLoaded = [&](const auto &m)
        {
            return m.find(type) != m.end();
        };

        if (isTypeLoaded(m_textures) || isTypeLoaded(m_texPaths) || isTypeLoaded(m_texNames))
            throw runtime_error("Texture with the same type already exists. Type: " + texture.key());

        const json &block = texture.value();

        if (block.contains(Dump)) {
            Texture2DManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.import(block[Dump]);
            m_textures[type] = manager;
        } else if (block.contains(Path)) {
            m_texPaths[type] = block[Path];
        } else if (block.contains(Name)) {
            m_texNames[type] = block[Name];
        } else {
            throw invalid_argument("Unknown Texture source '" + texture.key() + "'");
        }
    }
}

JsonHelper AMTLMaterialManager::dump() {
    using namespace Config;

    json config;

    // write name
    if (!m_name.empty())
        config[Name] = m_name;

    // write properties
    auto writePropImpl = [&](const string &key, float value, float defaultValue)
    {
        if (value != defaultValue) {
            config[key] = value;
        }
    };

#define writeProp(name, value) writePropImpl(name, value, Default::name)

    writeProp(AmbientStrength, m_ambientStrength);
    writeProp(DiffuseStrength, m_diffuseStrength);
    writeProp(SpecularStrength, m_specularStrength);
    writeProp(Reflection, m_reflection);
    writeProp(Jitter, m_jitter);
    writeProp(Shininess, m_shininess);

#undef writeProp

    // write textures
    json &textures = config[Textures];

    for (auto & texture : m_textures)
        textures[textureTypeToString(texture.first)][Dump] = texture.second.dump().json;

    for (auto & name : m_texNames)
        textures[textureTypeToString(name.first)][Name] = name.second;

    for (auto & path : m_texPaths)
        textures[textureTypeToString(path.first)][Path] = path.second;

    return config;
}
}

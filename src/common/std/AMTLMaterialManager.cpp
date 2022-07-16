#include <algine/std/AMTLMaterialManager.h>
#include <algine/core/texture/Texture2D.h>

using namespace std;

namespace algine {
AMTLMaterialManager::AMTLMaterialManager() = default;

AMTLMaterialManager::AMTLMaterialManager(string_view name)
    : m_name(name) {}

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

    return hasTex(m_textures) || hasTex(m_texNames);
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

set<string> AMTLMaterialManager::collectTextureNames() const {
    set<string> names;

    auto collect = [&](const auto &m) {
        for (const auto &p : m) {
            names.insert(p.first);
        }
    };

    collect(m_textures);
    collect(m_texNames);

    return names;
}

Texture2DPtr AMTLMaterialManager::loadTexture(const string &texName) {
    if (auto it = m_textures.find(texName); it != m_textures.end()) {
        it->second.setIOSystem(io());
        return it->second.get();
    }

    if (auto it = m_texNames.find(texName); it != m_texNames.end()) {
        return Texture2D::getByName(it->second);
    }

    return nullptr;
}

void AMTLMaterialManager::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "AMTLMaterialManager"))
        return;

    lua->registerUsertype<IOProvider>(tenv);

    auto ctors = sol::constructors<AMTLMaterialManager(), AMTLMaterialManager(std::string)>();
    auto usertype = env.new_usertype<AMTLMaterialManager>(
            "AMTLMaterialManager",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider>());

    Lua::new_property(usertype, "name", &AMTLMaterialManager::getName, &AMTLMaterialManager::setName);

    usertype["setFloat"] = &AMTLMaterialManager::setFloat;
    usertype["setTexture"] = &AMTLMaterialManager::setTexture;
    usertype["setTextureName"] = &AMTLMaterialManager::setTextureName;
    usertype["getFloat"] = &AMTLMaterialManager::getFloat;
    usertype["hasFloat"] = &AMTLMaterialManager::hasFloat;
    usertype["hasTexture"] = &AMTLMaterialManager::hasTexture;
    usertype["getFloats"] = &AMTLMaterialManager::getFloats;
    usertype["getTextures"] = &AMTLMaterialManager::getTextures;
    usertype["getTextureNames"] = &AMTLMaterialManager::getTextureNames;
    usertype["collectTextureNames"] = &AMTLMaterialManager::collectTextureNames;
    usertype["loadTexture"] = &AMTLMaterialManager::loadTexture;
}

void AMTLMaterialManager::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<AMTLMaterialManager>(s, path, lua, tenv);
}
}

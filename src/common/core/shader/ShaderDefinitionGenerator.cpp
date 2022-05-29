#include <algine/core/shader/ShaderDefinitionGenerator.h>

#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Definitions, "definitions");
constant(Params, "params");
}

namespace algine {
inline int getMacroIndexImpl(const vector<ShaderDefinitionGenerator::Definition> &definitions,
                             const std::string &macro)
{
    for (int i = 0; i < definitions.size(); i++) {
        if (definitions[i].first == macro) {
            return i;
        }
    }

    return -1;
}

#define getMacroIndex(macro) getMacroIndexImpl(m_definitions, macro)

void ShaderDefinitionGenerator::define(const string &macro, const string &value) {
    int index = getMacroIndex(macro);

    if (index == -1) {
        m_definitions.emplace_back(macro, value);
    } else {
        m_definitions[index].second = value; // overwrite existing
    }
}

void ShaderDefinitionGenerator::define(const string &macro, size value) {
    define(macro, to_string(value));
}

void ShaderDefinitionGenerator::removeDefinition(const string &macro) {
    int index = getMacroIndex(macro);

    if (index != -1) {
        m_definitions.erase(m_definitions.begin() + index);
    }
}

void ShaderDefinitionGenerator::resetDefinitions() {
    m_definitions.clear();
}

void ShaderDefinitionGenerator::setDefinitions(const vector<Definition> &definitions) {
    m_definitions = definitions;
}

void ShaderDefinitionGenerator::appendDefinitions(const vector<Definition> &definitions) {
    for (const auto & def : definitions) {
        define(def.first, def.second);
    }
}

const vector<ShaderDefinitionGenerator::Definition>& ShaderDefinitionGenerator::getDefinitions() const {
    return m_definitions;
}

void ShaderDefinitionGenerator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load definitions
    if (config.contains(Definitions)) {
        const json &defs = config[Definitions];

        for (const auto &def : defs.items()) {
            m_definitions.emplace_back(def.key(), def.value());
        }
    }

    // load params
    if (config.contains(Params)) {
        const json &params = config[Params];

        for (const auto &param : params) {
            m_definitions.emplace_back(param, "");
        }
    }
}

JsonHelper ShaderDefinitionGenerator::dump() {
    using namespace Config;

    json config;

    for (const auto & def : m_definitions) {
        if (def.second.empty()) {
            config[Params].emplace_back(def.first); // if value empty - it is param, not definition
        } else {
            config[Definitions][def.first] = def.second;
        }
    }

    return config;
}

void ShaderDefinitionGenerator::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "ShaderDefinitionGenerator"))
        return;

    Scriptable::registerLuaUsertype(lua);

    auto &state = *lua->state();

    auto usertype = state.new_usertype<ShaderDefinitionGenerator>(
            "ShaderDefinitionGenerator", sol::base_classes, sol::bases<Scriptable>());

    Lua::new_property(usertype, "definitions", "getDefinitions", "setDefinitions",
        &ShaderDefinitionGenerator::getDefinitions,
        [](const sol::object &self, vector<Definition> defs) { self.as<ShaderDefinitionGenerator>().setDefinitions(defs); });

    usertype["define"] = static_cast<void (ShaderDefinitionGenerator::*)(const string&, const string&)>(&ShaderDefinitionGenerator::define);
    usertype["removeDefinition"] = &ShaderDefinitionGenerator::removeDefinition;
    usertype["resetDefinitions"] = &ShaderDefinitionGenerator::resetDefinitions;
    usertype["appendDefinitions"] = [](const sol::object &self, vector<Definition> defs) { self.as<ShaderDefinitionGenerator>().appendDefinitions(defs); };

    auto defCtors = sol::constructors<Definition(), Definition(const std::string&, const std::string&)>();
    auto defUsertype = state["ShaderDefinitionGenerator"].get<sol::table>().new_usertype<Definition>(
            "Definition",
            sol::meta_function::construct, defCtors,
            sol::call_constructor, defCtors);
    defUsertype["key"] = &Definition::first;
    defUsertype["value"] = &Definition::second;
}

void ShaderDefinitionGenerator::exec(const std::string &s, bool path, Lua *lua) {
    exec_t<ShaderDefinitionGenerator>(s, path, lua);
}
}

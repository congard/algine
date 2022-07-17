#include <algine/core/shader/ShaderDefinitionGenerator.h>

using namespace std;
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

void ShaderDefinitionGenerator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "ShaderDefinitionGenerator"))
        return;

    Scriptable::registerLuaUsertype(lua, tenv);

    auto usertype = env.new_usertype<ShaderDefinitionGenerator>(
            "ShaderDefinitionGenerator", sol::base_classes, sol::bases<Scriptable>());

    Lua::new_property(usertype, "definitions",
        &ShaderDefinitionGenerator::getDefinitions,
        [](const sol::object &self, vector<Definition> defs) { self.as<ShaderDefinitionGenerator>().setDefinitions(defs); });

    usertype["define"] = sol::overload(
        static_cast<void (ShaderDefinitionGenerator::*)(const string&, const string&)>(&ShaderDefinitionGenerator::define),
        static_cast<void (ShaderDefinitionGenerator::*)(const string&, size)>(&ShaderDefinitionGenerator::define),
        [](ShaderDefinitionGenerator &self, const string &macro) { self.define(macro); });
    usertype["removeDefinition"] = &ShaderDefinitionGenerator::removeDefinition;
    usertype["resetDefinitions"] = &ShaderDefinitionGenerator::resetDefinitions;
    usertype["appendDefinitions"] = [](const sol::object &self, vector<Definition> defs) { self.as<ShaderDefinitionGenerator>().appendDefinitions(defs); };

    auto defCtors = sol::constructors<Definition(), Definition(const std::string&, const std::string&)>();
    auto defUsertype = env["ShaderDefinitionGenerator"].get<sol::table>().new_usertype<Definition>(
            "Definition",
            sol::meta_function::construct, defCtors,
            sol::call_constructor, defCtors);
    defUsertype["key"] = &Definition::first;
    defUsertype["value"] = &Definition::second;
}

void ShaderDefinitionGenerator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<ShaderDefinitionGenerator>(s, path, lua, tenv);
}
}

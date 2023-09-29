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

void ShaderDefinitionGenerator::exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
    exec_t<ShaderDefinitionGenerator>(s, path, lua, tenv);
}
}

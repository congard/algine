#include <algine/core/shader/ShaderDefinitionManager.h>

#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Definitions, "definitions");
constant(Params, "params");
}

namespace algine {
inline int getMacroIndexImpl(const vector<ShaderDefinitionManager::Definition> &definitions,
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

void ShaderDefinitionManager::define(const string &macro, const string &value) {
    int index = getMacroIndex(macro);

    if (index == -1) {
        m_definitions.emplace_back(macro, value);
    } else {
        m_definitions[index].second = value; // overwrite existing
    }
}

void ShaderDefinitionManager::define(const string &macro, size value) {
    define(macro, to_string(value));
}

void ShaderDefinitionManager::removeDefinition(const string &macro) {
    int index = getMacroIndex(macro);

    if (index != -1) {
        m_definitions.erase(m_definitions.begin() + index);
    }
}

void ShaderDefinitionManager::resetDefinitions() {
    m_definitions.clear();
}

void ShaderDefinitionManager::setDefinitions(const vector<Definition> &definitions) {
    m_definitions = definitions;
}

void ShaderDefinitionManager::appendDefinitions(const vector<Definition> &definitions) {
    for (const auto & def : definitions) {
        define(def.first, def.second);
    }
}

const vector<ShaderDefinitionManager::Definition>& ShaderDefinitionManager::getDefinitions() const {
    return m_definitions;
}

void ShaderDefinitionManager::import(const JsonHelper &jsonHelper) {
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

JsonHelper ShaderDefinitionManager::dump() {
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
}

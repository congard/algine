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
void ShaderDefinitionManager::define(const string &macro, const string &value) {
    m_definitions.emplace_back(macro, value);
}

void ShaderDefinitionManager::define(const string &macro, size value) {
    define(macro, to_string(value));
}

void ShaderDefinitionManager::removeDefinition(const string &macro, uint type) {
    switch (type) {
        case RemoveFirst:
        case RemoveAll:
            for (uint i = 0; i < static_cast<uint>(m_definitions.size()); i++)
                if (m_definitions[i].first == macro) {
                    m_definitions.erase(m_definitions.begin() + i);

                    if (type == RemoveFirst)
                        return;
                }
            break;
        case RemoveLast:
            for (int i = static_cast<int>(m_definitions.size()) - 1; i >= 0; i--)
                if (m_definitions[i].first == macro) {
                    m_definitions.erase(m_definitions.begin() + i);
                    return;
                }
            break;
        default:
            throw runtime_error("Unknown remove type " + to_string(type));
    }
}

void ShaderDefinitionManager::resetDefinitions() {
    m_definitions.clear();
}

void ShaderDefinitionManager::setDefinitions(const vector<Definition> &definitions) {
    m_definitions = definitions;
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

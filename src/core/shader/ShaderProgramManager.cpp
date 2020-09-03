#include <algine/core/shader/ShaderProgramManager.h>

#include <algine/core/JsonHelper.h>

#include <tulz/File.h>
#include <iostream>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Shaders, "shaders");
constant(Path, "path");
constant(Dump, "dump");
constant(Name, "name");
}

namespace algine {
void ShaderProgramManager::setPrivateShaders(const vector<PrivateShader> &shaders) {
    m_privateShaders = shaders;
}

void ShaderProgramManager::addPrivateShader(const ShaderManager &manager, uint dumpMode) {
    m_privateShaders.emplace_back(manager, dumpMode);
}

const vector<ShaderProgramManager::PrivateShader>& ShaderProgramManager::getPrivateShaders() const {
    return m_privateShaders;
}

void ShaderProgramManager::setPublicShaders(const vector<string> &shaders) {
    m_publicShaders = shaders;
}

void ShaderProgramManager::addPublicShader(const string &name) {
    m_publicShaders.emplace_back(name);
}

const vector<string>& ShaderProgramManager::getPublicShaders() const {
    return m_publicShaders;
}

ShaderProgramPtr ShaderProgramManager::createProgram() {
    ShaderProgramPtr program = make_shared<ShaderProgram>();
    program->setName(m_name);

    for (auto & helper : m_privateShaders) {
        auto &manager = helper.object;

        if (manager.getAccess() == ShaderManager::Access::Public)
            throw runtime_error("Can't use public Shader as private");

        // backup Shader level definitions
        auto shaderDefs = manager.getDefinitions();

        // append ShaderProgram level definitions
        manager.appendDefinitions(m_definitions);

        auto shader = manager.createShader();
        program->attachShader(*shader);

        // restore Shader level definitions
        manager.setDefinitions(shaderDefs);
    }

    for (auto & name : m_publicShaders) {
        auto shader = Shader::byName(name);

        if (shader == nullptr)
            throw runtime_error("Public Shader '" + name + "' does not exist");

        program->attachShader(*shader);
    }

    if (m_access == Access::Public) {
        if (m_name.empty())
            throw runtime_error("ShaderProgram without name can't be public");

        if (ShaderProgram::byName(m_name) == nullptr) {
            ShaderProgram::publicPrograms.emplace_back(program);
        } else {
            throw runtime_error("ShaderProgram with the same name was already loaded");
        }
    }

    program->link();

    return program;
}

void ShaderProgramManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &shaders = jsonHelper.json[Shaders];

    // load private & public shaders
    for (const auto & shader : shaders) {
        if (shader.contains(Path)) {
            ShaderManager shaderManager;
            shaderManager.importFromFile(shader[Path]);
            m_privateShaders.emplace_back(shaderManager);
        } else if (shader.contains(Dump)) {
            ShaderManager shaderManager;
            shaderManager.import(shader[Dump]);
            m_privateShaders.emplace_back(shaderManager);
        } else if (shader.contains(Name)) {
            m_publicShaders.emplace_back(shader[Name]);
        } else {
            throw invalid_argument("Unknown Shader source:\n" + shader.dump(4));
        }
    }

    ManagerBase::import(jsonHelper);
    ShaderDefinitionManager::import(jsonHelper);
}

JsonHelper ShaderProgramManager::dump() {
    using namespace Config;

    json config;

    auto emplaceData = [&](const string &key, const json &value)
    {
        json kv;
        kv[key] = value;
        config[Shaders].emplace_back(kv);
    };

    // write private shaders
    for (auto & mgrHelper : m_privateShaders) {
        const auto mode = mgrHelper.dumpMode;
        auto &shaderManager = mgrHelper.object;

        if (mode == PrivateShader::Path) {
            if (shaderManager.getConfigPath().empty()) {
                cerr << "Warning: ShaderProgramManager::dump(): Shader path is empty, will be used dump instead\n";
                emplaceData(Dump, shaderManager.dump().json);
            } else {
                emplaceData(Path, shaderManager.getConfigPath());
            }
        } else if (mode == PrivateShader::Dump) {
            emplaceData(Dump, shaderManager.dump().json);
        } else {
            throw invalid_argument("Unknown PrivateShader::dumpMode '" + to_string(mode) + "'");
        }
    }

    // write public shaders
    for (auto & name : m_publicShaders)
        emplaceData(Name, name);

    JsonHelper result(config);
    result.append(ManagerBase::dump());
    result.append(ShaderDefinitionManager::dump());

    return result;
}

void ShaderProgramManager::importFromFile(const std::string &path) {
    import(tulz::File(path, tulz::File::ReadText).readStr());
}
}

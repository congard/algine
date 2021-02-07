#include <algine/core/shader/ShaderProgramManager.h>

#include <algine/core/JsonHelper.h>

#include <iostream>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace nlohmann;
using namespace algine::internal;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Shaders, "shaders");
constant(Path, "path");
constant(Dump, "dump");
constant(Name, "name");
}

namespace algine {
void ShaderProgramManager::setShaders(const vector<ShaderManager> &shaders) {
    m_shaders = shaders;
}

void ShaderProgramManager::addShader(const ShaderManager &manager) {
    m_shaders.emplace_back(manager);
}

const vector<ShaderManager>& ShaderProgramManager::getShaders() const {
    return m_shaders;
}

void ShaderProgramManager::setShaderNames(const vector<string> &names) {
    m_shaderNames = names;
}

void ShaderProgramManager::addShaderName(const string &name) {
    m_shaderNames.emplace_back(name);
}

const vector<string>& ShaderProgramManager::getShaderNames() const {
    return m_shaderNames;
}

void ShaderProgramManager::setShaderPaths(const vector<string> &paths) {
    m_shaderPaths = paths;
}

void ShaderProgramManager::addShaderPath(const string &path) {
    m_shaderPaths.emplace_back(path);
}

const vector<string>& ShaderProgramManager::getShaderPaths() const {
    return m_shaderPaths;
}

ShaderProgramPtr ShaderProgramManager::get() {
    return PublicObjectTools::getPtr<ShaderProgramPtr>(this);
}

ShaderProgramPtr ShaderProgramManager::create() {
    ShaderProgramPtr program = make_shared<ShaderProgram>();
    program->setName(m_name);

    auto processMixedShader = [&](ShaderManager &manager)
    {
        if (manager.getAccess() == ShaderManager::Access::Public) {
            // note that if Shader public, then ShaderProgram
            // level definitions will not be applied

            auto publicShader = Shader::byName(manager.getName());

            if (publicShader != nullptr) {
                // if Shader already loaded, just use it
                program->attachShader(*publicShader);
            } else {
                // otherwise create it
                auto shader = manager.create();
                program->attachShader(*shader);
            }
        } else {
            // backup Shader level definitions
            auto shaderDefs = manager.getDefinitions();

            // append ShaderProgram level definitions
            manager.appendDefinitions(m_definitions);

            auto shader = manager.create();
            program->attachShader(*shader);

            // restore Shader level definitions
            manager.setDefinitions(shaderDefs);
        }
    };

    for (auto & manager : m_shaders)
        processMixedShader(manager);

    for (auto & path : m_shaderPaths) {
        ShaderManager manager;
        manager.setWorkingDirectory(m_workingDirectory);
        manager.importFromFile(path);
        processMixedShader(manager);
    }

    for (auto & name : m_shaderNames) {
        auto shader = Shader::byName(name);

        if (shader == nullptr)
            throw runtime_error("Public Shader '" + name + "' does not exist");

        program->attachShader(*shader);
    }

    program->link();

    PublicObjectTools::postCreateAccessOp("ShaderProgram", this, program);

    return program;
}

void ShaderProgramManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &shaders = jsonHelper.json[Shaders];

    // load shaders
    for (const auto & shader : shaders) {
         if (shader.contains(Dump)) {
            ShaderManager shaderManager;
            shaderManager.setWorkingDirectory(m_workingDirectory);
            shaderManager.import(shader[Dump]);
            m_shaders.emplace_back(shaderManager);
        } else if (shader.contains(Path)) {
             m_shaderPaths.emplace_back(shader[Path]);
        } else if (shader.contains(Name)) {
            m_shaderNames.emplace_back(shader[Name]);
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

    // write shaders
    for (auto & manager : m_shaders)
        emplaceData(Dump, manager.dump().json);

    for (auto & path : m_shaderPaths)
        emplaceData(Path, path);

    for (auto & name : m_shaderNames)
        emplaceData(Name, name);

    JsonHelper result(config);
    result.append(ManagerBase::dump());
    result.append(ShaderDefinitionManager::dump());

    return result;
}

void ShaderProgramManager::importFromFile(const std::string &path) {
    ManagerBase::importFromFile(path);
}
}

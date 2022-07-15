#include <algine/core/shader/ShaderProgramCreator.h>

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
void ShaderProgramCreator::setCreators(const vector<ShaderCreator> &shaders) {
    m_shaders = shaders;
}

void ShaderProgramCreator::addCreator(const ShaderCreator &creator) {
    m_shaders.emplace_back(creator);
}

const vector<ShaderCreator>& ShaderProgramCreator::getCreators() const {
    return m_shaders;
}

void ShaderProgramCreator::setShaderNames(const vector<string> &names) {
    m_shaderNames = names;
}

void ShaderProgramCreator::addShaderName(const string &name) {
    m_shaderNames.emplace_back(name);
}

const vector<string>& ShaderProgramCreator::getShaderNames() const {
    return m_shaderNames;
}

void ShaderProgramCreator::setShaderPaths(const vector<string> &paths) {
    m_shaderPaths = paths;
}

void ShaderProgramCreator::addShaderPath(const string &path) {
    m_shaderPaths.emplace_back(path);
}

const vector<string>& ShaderProgramCreator::getShaderPaths() const {
    return m_shaderPaths;
}

ShaderProgramPtr ShaderProgramCreator::get() {
    return PublicObjectTools::getPtr<ShaderProgramPtr>(this);
}

ShaderProgramPtr ShaderProgramCreator::create() {
    ShaderProgramPtr program = make_shared<ShaderProgram>();
    program->setName(m_name);

    auto processMixedShader = [&](ShaderCreator &creator)
    {
        if (creator.getAccess() == ShaderCreator::Access::Public) {
            // note that if Shader public, then ShaderProgram
            // level definitions will not be applied

            auto publicShader = Shader::byName(creator.getName());

            if (publicShader != nullptr) {
                // if Shader already loaded, just use it
                program->attachShader(*publicShader);
            } else {
                // otherwise create it
                auto shader = creator.create();
                program->attachShader(*shader);
            }
        } else {
            // backup Shader level definitions
            auto shaderDefs = creator.getDefinitions();

            // append ShaderProgram level definitions
            creator.appendDefinitions(m_definitions);

            auto shader = creator.create();
            program->attachShader(*shader);

            // restore Shader level definitions
            creator.setDefinitions(shaderDefs);
        }
    };

    for (auto &creator : m_shaders) {
        creator.setIOSystem(io());
        processMixedShader(creator);
    }

    for (auto &path : m_shaderPaths) {
        ShaderCreator creator;
        creator.setIOSystem(io());
        creator.setWorkingDirectory(m_workingDirectory);
        creator.importFromFile(path);
        processMixedShader(creator);
    }

    for (auto &name : m_shaderNames) {
        auto shader = Shader::byName(name);

        if (shader == nullptr)
            throw runtime_error("Public Shader '" + name + "' does not exist");

        program->attachShader(*shader);
    }

    program->link();

    PublicObjectTools::postCreateAccessOp("ShaderProgram", this, program);

    return program;
}

void ShaderProgramCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &shaders = jsonHelper.json[Shaders];

    // load shaders
    for (const auto & shader : shaders) {
         if (shader.contains(Dump)) {
            ShaderCreator shaderCreator;
            shaderCreator.setWorkingDirectory(m_workingDirectory);
            shaderCreator.import(shader[Dump]);
            m_shaders.emplace_back(shaderCreator);
        } else if (shader.contains(Path)) {
             m_shaderPaths.emplace_back(shader[Path]);
        } else if (shader.contains(Name)) {
            m_shaderNames.emplace_back(shader[Name]);
        } else {
            throw invalid_argument("Unknown Shader source:\n" + shader.dump(4));
        }
    }

    Creator::import(jsonHelper);
    ShaderDefinitionGenerator::import(jsonHelper);
}

JsonHelper ShaderProgramCreator::dump() {
    using namespace Config;

    json config;

    auto emplaceData = [&](const string &key, const json &value)
    {
        json kv;
        kv[key] = value;
        config[Shaders].emplace_back(kv);
    };

    // write shaders
    for (auto & creator : m_shaders)
        emplaceData(Dump, creator.dump().json);

    for (auto & path : m_shaderPaths)
        emplaceData(Path, path);

    for (auto & name : m_shaderNames)
        emplaceData(Name, name);

    JsonHelper result(config);
    result.append(Creator::dump());
    result.append(ShaderDefinitionGenerator::dump());

    return result;
}

void ShaderProgramCreator::importFromFile(const std::string &path) {
    Creator::importFromFile(path);
}

void ShaderProgramCreator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "ShaderProgramCreator"))
        return;

    lua->registerUsertype<ShaderProgram, ShaderCreator, ShaderDefinitionGenerator>(tenv);

    auto ctors = sol::constructors<ShaderProgramCreator()>();
    auto usertype = env.new_usertype<ShaderProgramCreator>(
            "ShaderProgramCreator",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider, FileTransferable, Creator, ShaderDefinitionGenerator>());

    Lua::new_property(usertype, "creators",
        &ShaderProgramCreator::getCreators,
        [](const sol::object &self, vector<ShaderCreator> creators) { self.as<ShaderProgramCreator>().setCreators(creators); });

    Lua::new_property(usertype, "names",
        &ShaderProgramCreator::getShaderNames,
        [](const sol::object &self, vector<string> names) { self.as<ShaderProgramCreator>().setShaderNames(names); });

    usertype["addCreator"] = &ShaderProgramCreator::addCreator;
    usertype["addName"] = &ShaderProgramCreator::addShaderName;
    usertype["get"] = &ShaderProgramCreator::get;
    usertype["create"] = &ShaderProgramCreator::create;
}

void ShaderProgramCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) {
    exec_t<ShaderProgramCreator>(s, path, lua, env);
}
}

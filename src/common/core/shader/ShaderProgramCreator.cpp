#include <algine/core/shader/ShaderProgramCreator.h>

#include <iostream>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;

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

ShaderProgramPtr ShaderProgramCreator::get() {
    return PublicObjectTools::getPtr<ShaderProgramPtr>(this);
}

ShaderProgramPtr ShaderProgramCreator::create() {
    ShaderProgramPtr program = make_shared<ShaderProgram>();
    program->setName(m_name);

    auto processMixedShader = [&](ShaderCreator &creator) {
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
            sol::base_classes, sol::bases<Scriptable, IOProvider, Creator, ShaderDefinitionGenerator>());

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

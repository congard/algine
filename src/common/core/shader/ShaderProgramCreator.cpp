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

void ShaderProgramCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) {
    exec_t<ShaderProgramCreator>(s, path, lua, env);
}
}

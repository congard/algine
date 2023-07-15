#include <algine/core/shader/ShaderProgramBuilder.h>

#include <iostream>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;

namespace algine {
void ShaderProgramBuilder::setBuilders(const std::vector<ShaderBuilder> &shaders) {
    m_shaders = shaders;
}

void ShaderProgramBuilder::addBuilder(const ShaderBuilder &builder) {
    m_shaders.emplace_back(builder);
}

const vector<ShaderBuilder>& ShaderProgramBuilder::getBuilders() const {
    return m_shaders;
}

void ShaderProgramBuilder::setShaderNames(const vector<string> &names) {
    m_shaderNames = names;
}

void ShaderProgramBuilder::addShaderName(const string &name) {
    m_shaderNames.emplace_back(name);
}

const vector<string>& ShaderProgramBuilder::getShaderNames() const {
    return m_shaderNames;
}

ShaderProgramPtr ShaderProgramBuilder::get() {
    return PublicObjectTools::getPtr<ShaderProgramPtr>(this);
}

ShaderProgramPtr ShaderProgramBuilder::create() {
    ShaderProgramPtr program = make_shared<ShaderProgram>();
    program->setName(m_name);

    auto processMixedShader = [&](ShaderBuilder &builder) {
        if (builder.getAccess() == ShaderBuilder::Access::Public) {
            // note that if Shader public, then ShaderProgram
            // level definitions will not be applied

            auto publicShader = Shader::byName(builder.getName());

            if (publicShader != nullptr) {
                // if Shader already loaded, just use it
                program->attachShader(*publicShader);
            } else {
                // otherwise create it
                auto shader = builder.create();
                program->attachShader(*shader);
            }
        } else {
            // backup Shader level definitions
            auto shaderDefs = builder.getDefinitions();

            // append ShaderProgram level definitions
            builder.appendDefinitions(m_definitions);

            auto shader = builder.create();
            program->attachShader(*shader);

            // restore Shader level definitions
            builder.setDefinitions(shaderDefs);
        }
    };

    for (auto &builder : m_shaders) {
        builder.setIOSystem(io());
        processMixedShader(builder);
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

void ShaderProgramBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) {
    exec_t<ShaderProgramBuilder>(s, path, lua, env);
}
}

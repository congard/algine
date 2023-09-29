#include <algine/core/shader/ShaderProgramBuilder.h>

#include <iostream>

using namespace std;

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

Object* ShaderProgramBuilder::createImpl() {
    auto program = new ShaderProgram(getActualParent());
    program->setName(m_name);

    auto processMixedShader = [&](ShaderBuilder &builder) {
        // Since ShaderProgram has its own definitions and Shader has its own,
        // simply using the name is not sufficient to find a shader. Therefore,
        // a new shader will be created as a child of the current shader program.

        // backup Shader definitions
        auto shaderDefs = builder.getDefinitions();

        // append ShaderProgram definitions
        builder.appendDefinitions(m_definitions);

        builder.setParent(program);

        auto shader = builder.create();
        program->attachShader(*shader);

        // restore Shader definitions
        builder.setDefinitions(shaderDefs);
    };

    for (auto &builder : m_shaders) {
        builder.setIOSystem(io());
        processMixedShader(builder);
    }

    for (auto &name : m_shaderNames) {
        auto shader = getActualParent()->findChild<Shader*>(name, Object::FindOption::DirectThisAndScene);

        if (shader == nullptr)
            throw runtime_error("Public Shader '" + name + "' does not exist");

        program->attachShader(*shader);
    }

    program->link();

    return program;
}

void ShaderProgramBuilder::exec(const std::string &s, bool path, Lua *lua, sol::environment *env) {
    exec_t<ShaderProgramBuilder>(s, path, lua, env);
}
}

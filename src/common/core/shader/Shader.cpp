#include <algine/core/shader/Shader.h>
#include <algine/core/shader/ShaderTools.h>
#include <algine/gl.h>

#include <tulz/File.h>
#include <iostream>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace tulz;
using namespace algine::internal;

namespace algine {
vector<ShaderPtr> Shader::publicObjects;

Shader::Shader(Type type) {
    create(type);
}

Shader::Shader() = default;

Shader::~Shader() {
    glDeleteShader(m_id);
}

void Shader::create(Type type) {
    switch (type) {
        case Type::Vertex:
            m_id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Type::Fragment:
            m_id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case Type::Geometry:
            m_id = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default:
            cerr << "Unknown shader type " << static_cast<int>(type) << "\n";
    }
}

void Shader::fromSource(const string &source) {
    const char *c_str = source.c_str();

    glShaderSource(m_id, 1, &c_str, nullptr);
    glCompileShader(m_id);

    string infoLog = ShaderTools::getShaderInfoLogById(m_id, GL_COMPILE_STATUS);

    if (!infoLog.empty()) {
        cerr << "Info log of Shader with id " << m_id << ": " << infoLog;
    }
}

void Shader::fromFile(const string &path) {
    fromSource(File(path, File::Mode::ReadText).readStr());
}

uint Shader::getId() const {
    return m_id;
}

void Shader::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "Shader"))
        return;

    lua->registerUsertype<Object>(tenv);

    auto factories = sol::factories(
            []() { return PtrMaker::make<Shader>(); },
            [](Type type) { return PtrMaker::make<Shader>(type); });

    auto usertype = env.new_usertype<Shader>(
            "Shader",
            sol::call_constructor, factories,
            sol::meta_function::construct, factories,
            sol::base_classes, sol::bases<Object>());
    usertype["create"] = &Shader::create;
    usertype["fromSource"] = &Shader::fromSource;
    usertype["fromFile"] = &Shader::fromFile;
    usertype["getId"] = &Shader::getId;

    // static
    usertype["getByName"] = &Shader::getByName;
    usertype["byName"] = &Shader::byName;
}

ShaderPtr Shader::getByName(string_view name) {
    return PublicObjectTools::getByName<ShaderPtr>(name);
}

Shader* Shader::byName(string_view name) {
    return PublicObjectTools::byName<Shader>(name);
}
}

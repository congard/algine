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

Shader::Shader(const uint type) {
    create(type);
}

Shader::Shader() = default;

Shader::~Shader() {
    glDeleteShader(m_id);
}

void Shader::create(const uint type) {
    switch (type) {
        case Vertex:
            m_id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Fragment:
            m_id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case Geometry:
            m_id = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default:
            cerr << "Unknown shader type " << type << "\n";
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

ShaderPtr Shader::getByName(const string &name) {
    return PublicObjectTools::getByName<ShaderPtr>(name);
}

Shader* Shader::byName(const string &name) {
    return PublicObjectTools::byName<Shader>(name);
}
}

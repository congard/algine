#include <algine/core/shader/Shader.h>
#include <algine/core/shader/ShaderTools.h>
#include <algine/gl.h>

#include <tulz/File.h>
#include <iostream>

using namespace std;
using namespace tulz;

namespace algine {
Shader::Shader(Type type, Object *parent)
    : Object(parent)
{
    create(type);
}

Shader::Shader(Object *parent)
    : Object(parent) {}

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
}

#include <algine/core/Shader.h>

#include <algine/core/ShaderTools.h>

#include <tulz/File>
#include <GL/glew.h>
#include <iostream>

using namespace std;
using namespace tulz;

namespace algine {
Shader::Shader(const uint type) {
    create(type);
}

Shader::Shader() = default;

Shader::~Shader() {
    glDeleteShader(id);
}

void Shader::create(const uint type) {
    switch (type) {
        case Vertex:
            id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case Fragment:
            id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case Geometry:
            id = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default:
            cerr << "Unknown shader type " << type << "\n";
    }
}

void Shader::fromSource(const string &source) {
    const char *c_str = source.c_str();
    glShaderSource(id, 1, &c_str, nullptr);
    glCompileShader(id);
    ShaderTools::getShaderInfoLogById(id, GL_COMPILE_STATUS);
}

void Shader::fromFile(const string &path) {
    fromSource(File(path, File::Read).readStr());
}
}

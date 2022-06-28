#include <algine/core/shader/ShaderTools.h>

#include <algine/core/shader/ShaderProgram.h>
#include <algine/gl.h>

#include <tulz/Array.h>

using namespace std;
using namespace tulz;

namespace algine::ShaderTools {
string getShaderInfoLogById(uint shader, uint type) {
    GLint infoLogSize, success;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize == 0)
        return {};

    Array<char> infoLog(infoLogSize);
    glGetShaderiv(shader, type, &success);

    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog.array());
        return infoLog.array();
    }

    return {};
}

string getProgramInfoLogById(uint program, uint type) {
    GLint infoLogSize, success;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize == 0)
        return {};

    Array<char> infoLog(infoLogSize);
    glGetProgramiv(program, type, &success);

    if (!success) {
        glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog.array());
        return infoLog.array();
    }

    return {};
}

string getShaderInfoLog(const Shader *shader, uint type) {
    return getShaderInfoLogById(shader->getId(), type);
}

string getProgramInfoLog(const ShaderProgram *program, uint type) {
    return getProgramInfoLogById(program->getId(), type);
}
}

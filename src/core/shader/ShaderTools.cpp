#include <algine/core/shader/ShaderTools.h>

#include <algine/core/shader/ShaderProgram.h>

#include <tulz/Array.h>
#include <GL/glew.h>

using namespace std;
using namespace tulz;

namespace algine {
namespace ShaderTools {
string getShaderInfoLogById(const uint shader, const uint type) {
    GLint infoLogSize, success;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize == 0)
        return string();

    Array<char> infoLog(infoLogSize);
    glGetShaderiv(shader, type, &success);

    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog.array());
        return string(infoLog.array());
    }

    return string();
}

string getProgramInfoLogById(const uint program, const uint type) {
    GLint infoLogSize, success;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize == 0)
        return string();

    Array<char> infoLog(infoLogSize);
    glGetProgramiv(program, type, &success);

    if (!success) {
        glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog.array());
        return string(infoLog.array());
    }

    return string();
}

string getShaderInfoLog(const Shader *const shader, const uint type) {
    return getShaderInfoLogById(shader->id, type);
}

string getProgramInfoLog(const ShaderProgram *const program, const uint type) {
    return getProgramInfoLogById(program->id, type);
}
}
}

#ifndef ALGINE_SHADERTOOLS_H
#define ALGINE_SHADERTOOLS_H

#include <algine/types.h>

#include <string>

class Shader;
class ShaderProgram;

namespace algine {
namespace ShaderTools {
std::string getShaderInfoLogById(uint shader, uint type);
std::string getProgramInfoLogById(uint program, uint type);
std::string getShaderInfoLog(const Shader *shader, uint type);
std::string getProgramInfoLog(const ShaderProgram *program, uint type);
}
}

#endif //ALGINE_SHADERTOOLS_H

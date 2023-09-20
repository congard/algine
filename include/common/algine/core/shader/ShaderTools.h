#ifndef ALGINE_SHADERTOOLS_H
#define ALGINE_SHADERTOOLS_H

#include <algine/platform.h>
#include <algine/types.h>

#include <string>

class Shader;
class ShaderProgram;

namespace algine::ShaderTools {
AL_EXPORT std::string getShaderInfoLogById(uint shader, uint type);
AL_EXPORT std::string getProgramInfoLogById(uint program, uint type);
AL_EXPORT std::string getShaderInfoLog(const Shader *shader, uint type);
AL_EXPORT std::string getProgramInfoLog(const ShaderProgram *program, uint type);
}

#endif //ALGINE_SHADERTOOLS_H

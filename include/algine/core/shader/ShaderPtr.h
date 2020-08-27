#ifndef ALGINE_SHADERPTR_H
#define ALGINE_SHADERPTR_H

#include <memory>

namespace algine {
class Shader;

typedef std::shared_ptr<Shader> ShaderPtr;
}

#endif //ALGINE_SHADERPTR_H

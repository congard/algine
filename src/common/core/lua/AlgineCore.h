#ifndef ALGINE_ALGINECORE_H
#define ALGINE_ALGINECORE_H

#include <algine/core/lua/Lua.h>

#include <algine/core/io/IOProvider.h>
#include <algine/core/log/Log.h>
#include <algine/core/shader/ShaderProgramCreator.h>

namespace algine {
using AlgineCore = Lua::TypeList<
        IOProvider,
        Log,
        ShaderProgramCreator, ShaderProgram, ShaderDefinitionGenerator, Shader, ShaderProgram,
        Creator,
        Object>;
}

#endif //ALGINE_ALGINECORE_H

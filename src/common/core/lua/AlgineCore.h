#ifndef ALGINE_ALGINECORE_H
#define ALGINE_ALGINECORE_H

#include <algine/core/lua/Lua.h>

#include <algine/core/io/IOProvider.h>
#include <algine/core/log/Log.h>
#include <algine/core/shader/ShaderProgramCreator.h>
#include <algine/core/ImageCreator.h>
#include <algine/core/texture/TextureCreator.h>
#include <algine/core/texture/Texture2DCreator.h>
#include <algine/core/texture/TextureCubeCreator.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/lua/DataType.h>

namespace algine {
using AlgineCore = Lua::TypeList<
        IOProvider,
        Log,
        ShaderProgramCreator, ShaderProgram, ShaderDefinitionGenerator, Shader, ShaderProgram,
        Creator,
        Object,
        lua::DataType,
        ImageCreator,
        TextureCreator, Texture2DCreator, TextureCubeCreator, Texture, Texture2D, TextureCube>;
}

#endif //ALGINE_ALGINECORE_H

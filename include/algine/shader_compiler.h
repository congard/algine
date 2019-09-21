#ifndef ALGINE_SHADER_COMPILER_H
#define ALGINE_SHADER_COMPILER_H

#include <string>
#include <vector>
#include <algine/shader_program.h>
#include <algine/types.h>

// TODO: pick up a new file name. This is far from a shader compiler

namespace algine {
namespace scompiler { // shader compiler
void saveShaders(const ShadersData &shader, const ShadersPaths &path);
void loadColorShaderLocations(ShaderProgram &shader, uint maxDirLightsCount, uint maxPointLightsCount);
void loadShadowShaderLocations(ShaderProgram &shader);
void loadDOFBlurShaderLocations(ShaderProgram &shader);
void loadDOFCoCShaderLocations(ShaderProgram &shader);
void loadBlendBloomShaderLocations(ShaderProgram &shader);
void loadSSRShaderLocations(ShaderProgram &shader);
void loadBloomSearchShaderLocations(ShaderProgram &shader);
void loadBlurShaderLocations(ShaderProgram &shader);
void loadCubemapShaderLocations(ShaderProgram &shader);
}
}

#endif

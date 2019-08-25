#ifndef ALGINE_SHADER_COMPILER_H
#define ALGINE_SHADER_COMPILER_H

#include <string>
#include <vector>
#include <algine/algine_structs.h>
#include <algine/shader_program.h>
#include <algine/types.h>

// TODO: pick up a new file name. This is far from a shader compiler
//  Remove get*: it is necessary to give this opportunity to the developer
//  Create something like ShaderMaker, which containing method define(key, value)

namespace algine {
namespace scompiler { // shader compiler
ShadersData readShader(const ShadersPaths &path);

// color shader
ShadersData getCS(const AlgineParams &params, const ColorShaderParams &csp, const char *vertexShaderPath, const char *fragmentShaderPath);

// shadow shader
ShadersData getSS(const AlgineParams &params, const ShadowShaderParams &ssp, const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath = nullptr);

// dof blur shader
std::vector<ShadersData> getDOFBlurShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath);

// dof coc shader
ShadersData getDOFCoCShader(const DOFBlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath);

// blend shader
ShadersData getBlendShader(const AlgineParams &params, const char *vertexShaderPath, const char *fragmentShaderPath);

// screen space reflections shader
ShadersData getSSRShader(const char *vertexShaderPath, const char *fragmentShaderPath);

// bloom search shader
ShadersData getBloomSearchShader(const char *vertexShaderPath, const char *fragmentShaderPath);

// returns `std::vector<ShadersData> { blurHorizontal, blurVertical }`
std::vector<ShadersData> getBlurShader(const BlurShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath);

// cubemap shader
ShadersData getCubemapShader(const CubemapShaderParams &params, const char *vertexShaderPath, const char *fragmentShaderPath);

void saveShaders(const ShadersData &shader, const ShadersPaths &path);
void loadColorShaderLocations(ShaderProgram &shader, const ColorShaderParams &csp);
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

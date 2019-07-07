#ifndef ALGINE_SHADER_COMPILER_H
#define ALGINE_SHADER_COMPILER_H

#include <string>
#include <vector>
#include <algine/algine_structs.h>
#include <algine/types.h>

namespace algine {
// shader compiler
namespace scompiler {

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

uint compileShader(const std::string &shader, const uint type);

// compiles vertex & fragment & geometry (if exists) shaders
std::vector<GLuint> compileShaders(const ShadersData &shader);

uint createShaderProgram(const std::vector<uint> &shaders);

// DS - delete shaders
uint createShaderProgramDS(const std::vector<uint> &shaders);

void loadLocations(CShader &shader, const AlgineParams &params, const ColorShaderParams &csp);

void loadLocations(SShader &shader);

void loadLocations(DOFBlurShader &shader);

void loadLocations(DOFCoCShader &shader);

void loadLocations(BlendShader &shader);

void loadLocations(SSRShader &shader);

void loadLocations(BloomSearchShader &shader);

void loadLocations(BlurShader &shader);

void loadLocations(CubemapShader &shader);
}
}

#endif
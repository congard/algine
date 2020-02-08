#ifndef ALGINE_RENDERER_H
#define ALGINE_RENDERER_H

#include <algine/types.h>
#include <algine/shader.h>
#include <algine/QuadRenderer.h>

namespace algine {
// TODO: clean up AlgineRenderer
class AlgineRenderer {
public:
    ShaderProgram *ssrShader = nullptr;
    ShaderProgram *bloomSearchShader = nullptr;
    ShaderProgram **blurShaders = nullptr; // { horizontal, vertical }
    ShaderProgram *blendShader = nullptr;
    ShaderProgram *dofBlurShaders[2] = {nullptr, nullptr}; // { horizontal, vertical }
    ShaderProgram *dofCoCShader = nullptr;

    QuadRenderer *quadRenderer = nullptr;

    bool horizontal = true, firstIteration = true; // blur variables

    void mainPass(uint displayFBO);
    void bloomSearchPass(uint bsFBO, uint image);
    void blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], uint image, uint blurAmount);
    void screenspacePass(uint ssFBO, uint colorMap, uint normalMap, uint ssrValuesMap, uint positionMap);
    void dofCoCPass(uint cocFBO, uint positionMap);

    // dofMap may be position map or coc map depending on the method you use
    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], uint image, uint dofMap, uint blurAmount);

    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], uint image, uint cocMap, uint positionMap, uint blurAmount);
    void doubleBlendPass(uint image, uint bloom);
    void blendPass(uint texture0);
};

} /* namespace algine */

#endif // ALGINE_RENDERER_H

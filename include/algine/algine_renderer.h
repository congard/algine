#ifndef ALGINE_RENDERER_H
#define ALGINE_RENDERER_H

#include <algine/types.h>
#include <algine/shader.h>

namespace algine {
void pointer(int location, int count, uint buffer, uint stride = 0, const void *offset = nullptr);
void pointerui(int location, int count, uint buffer, uint stride = 0, const void *offset = nullptr);

class CubeRenderer {
public:
    uint cubeBuffer, cubeVAO;

    void init(int inPosLocation = -1); // if `inPosLocation` != -1, VAO will be created
    void bindVAO();
    void drawCube(); // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 14)`
    void render();
    void render(int inPosLocation);
    void render(int programId, int inPosLocation);

    ~CubeRenderer();
};

class QuadRenderer {
public:
    uint quadBuffers[2]; // vertexBuffer and texCoordsBuffer
    uint quadVAO;

    void init(int inPosLocation = -1, int inTexCoordLocation = -1);
    void bindVAO();
    void drawQuad(); // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)`
    void render();
    void render(int inPosLocation, int inTexCoordLocation);
    void render(int programId, int inPosLocation, int inTexCoordLocation);

    ~QuadRenderer();
};

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

    // configures renderbuffer for main pass rendering
    // note: this function will bind empty renderbuffer (`bindRenderbuffer(0)`)
    void configureMainPassRenderbuffer(uint renderbuffer, uint width, uint height);

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

    ~AlgineRenderer();
};

} /* namespace algine */

#endif // ALGINE_RENDERER_H

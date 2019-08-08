#ifndef ALGINE_RENDERER_H
#define ALGINE_RENDERER_H

#include <algine/types.h>
#include <algine/algine_structs.h>

// uint location, glm::vec3 vec
#define setVec3(location, vec) glUniform3f(location, vec.x, vec.y, vec.z)
// uint location, glm::mat4 mat
#define setMat4(location, mat) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))

namespace algine {
void pointer(const int location, const int count, const uint buffer, const uint stride = 0, const void *offset = nullptr);
void pointerui(const int location, const int count, const uint buffer, const uint stride = 0, const void *offset = nullptr);

class CubeRenderer {
public:
    uint cubeBuffer, cubeVAO;

    void init(const int inPosLocation = -1); // if `inPosLocation` != -1, VAO will be created
    void bindVAO();
    void drawCube(); // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 14)`
    void render();
    void render(const int inPosLocation);
    void render(const int programId, const int inPosLocation);

    ~CubeRenderer();
};

class QuadRenderer {
public:
    uint quadBuffers[2]; // vertexBuffer and texCoordsBuffer
    uint quadVAO;

    void init(const int inPosLocation = -1, const int inTexCoordLocation = -1);
    void bindVAO();
    void drawQuad(); // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)`
    void render();
    void render(const int inPosLocation, const int inTexCoordLocation);
    void render(const int programId, const int inPosLocation, const int inTexCoordLocation);

    ~QuadRenderer();
};

class AlgineRenderer {
public:
    SSRShader *ssrs;
    BloomSearchShader *bloomSearchShader;
    BlurShader **blurShaders; // { horizontal, vertical }
    BlendShader *blendShader;
    DOFBlurShader *dofBlurShaders[2]; // { horizontal, vertical }
    DOFCoCShader *dofCoCShader;

    QuadRenderer *quadRenderer;

    bool horizontal = true, firstIteration = true; // blur variables

    // configures renderbuffer for main pass rendering
    // note: this function will bind empty renderbuffer (`bindRenderbuffer(0)`)
    void configureMainPassRenderbuffer(const uint renderbuffer, const uint width, const uint height);

    void mainPass(const uint displayFBO);
    void bloomSearchPass(const uint bsFBO, const uint image);
    void blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], const uint image, const uint blurAmount);
    void screenspacePass(const uint ssFBO, const uint colorMap, const uint normalMap, const uint ssrValuesMap, const uint positionMap);
    void dofCoCPass(const uint cocFBO, const uint positionMap);

    // dofMap may be position map or coc map depending on the method you use
    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint image, const uint dofMap, const uint blurAmount);

    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint image, const uint cocMap, const uint positionMap, const uint blurAmount);
    void doubleBlendPass(const uint image, const uint bloom);
    void blendPass(const uint texture0);

    ~AlgineRenderer();
};

} /* namespace algine */

#endif // ALGINE_RENDERER_H

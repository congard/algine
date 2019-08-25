#include <algine/algine_renderer.h>

#include <GL/glew.h>
#include <algine/texture.h>
#include <algine/framebuffer.h>
#include <algine/renderbuffer.h>
#include <algine/constants.h>

namespace algine {
void pointer(const int location, const int count, const uint buffer, const uint stride, const void *offset) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        location, // attribute location
        count,    // count (1, 2, 3 or 4)
        GL_FLOAT, // type
        GL_FALSE, // is normalized?
        stride,   // step
        offset    // offset
    );
}

void pointerui(const int location, const int count, const uint buffer, const uint stride, const void *offset) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribIPointer(
        location,        // attribute location
        count,           // count (1, 2, 3 or 4)
        GL_UNSIGNED_INT, // type
        stride,          // step
        offset           // offset
    );
}

// if `inPosLocation` != -1, VAO will be created
void CubeRenderer::init(const int inPosLocation) {
    // source: https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
    static const float vertices[] = {
        -1.0f,  1.0f,  1.0f,    // Front-top-left
         1.0f,  1.0f,  1.0f,    // Front-top-right
        -1.0f, -1.0f,  1.0f,    // Front-bottom-left
         1.0f, -1.0f,  1.0f,    // Front-bottom-right
         1.0f, -1.0f, -1.0f,    // Back-bottom-right
         1.0f,  1.0f,  1.0f,    // Front-top-right
         1.0f,  1.0f, -1.0f,    // Back-top-right
        -1.0f,  1.0f,  1.0f,    // Front-top-left
        -1.0f,  1.0f, -1.0f,    // Back-top-left
        -1.0f, -1.0f,  1.0f,    // Front-bottom-left
        -1.0f, -1.0f, -1.0f,    // Back-bottom-left
         1.0f, -1.0f, -1.0f,    // Back-bottom-right
        -1.0f,  1.0f, -1.0f,    // Back-top-left
         1.0f,  1.0f, -1.0f     // Back-top-right
    };

    glGenBuffers(1, &cubeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (inPosLocation == -1) return;

    // create & configure VAO
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glEnableVertexAttribArray(inPosLocation);
    pointer(inPosLocation, 3, cubeBuffer);

    glBindVertexArray(0);
}

void CubeRenderer::bindVAO() {
    glBindVertexArray(cubeVAO);
}

// just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 14)`
void CubeRenderer::drawCube() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
}

void CubeRenderer::render() {
    bindVAO();
    drawCube();
}

void CubeRenderer::render(const int inPosLocation) {
    glEnableVertexAttribArray(inPosLocation);
    pointer(inPosLocation, 3, cubeBuffer);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

    glDisableVertexAttribArray(inPosLocation);
}

void CubeRenderer::render(const int programId, const int inPosLocation) {
    glUseProgram(programId);
    render(inPosLocation);
}

CubeRenderer::~CubeRenderer() {
    glDeleteBuffers(1, &cubeBuffer);
    glDeleteVertexArrays(1, &cubeVAO);
}

void QuadRenderer::init(const int inPosLocation, const int inTexCoordLocation) {
    // creating buffers for quad rendering
    static float
        vertices[12] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        },

        texCoords[8] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

    glGenBuffers(2, quadBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffers[0]); // vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffers[1]); // texCoords
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    if (inPosLocation == -1 || inTexCoordLocation == -1) return;

    // create & configure VAO
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glEnableVertexAttribArray(inPosLocation);
    glEnableVertexAttribArray(inTexCoordLocation);
    pointer(inPosLocation, 3, quadBuffers[0]);
	pointer(inTexCoordLocation, 2, quadBuffers[1]);

    glBindVertexArray(0);
}

void QuadRenderer::bindVAO() {
    glBindVertexArray(quadVAO);
}

// just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)`
void QuadRenderer::drawQuad() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void QuadRenderer::render() {
    bindVAO();
    drawQuad();
}

void QuadRenderer::render(const int inPosLocation, const int inTexCoordLocation) {
	glEnableVertexAttribArray(inPosLocation);
    glEnableVertexAttribArray(inTexCoordLocation);
    pointer(inPosLocation, 3, quadBuffers[0]);
	pointer(inTexCoordLocation, 2, quadBuffers[1]);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
	glDisableVertexAttribArray(inPosLocation);
    glDisableVertexAttribArray(inTexCoordLocation);
}

void QuadRenderer::render(const int programId, const int inPosLocation, const int inTexCoordLocation) {
    glUseProgram(programId);
    render(inPosLocation, inTexCoordLocation);
}

QuadRenderer::~QuadRenderer() {
    glDeleteBuffers(2, quadBuffers);
    glDeleteVertexArrays(1, &quadVAO);
}

// configures renderbuffer for main pass rendering
// note: this function will bind empty renderbuffer (`bindRenderbuffer(0)`)
void AlgineRenderer::configureMainPassRenderbuffer(const uint renderbuffer, const uint width, const uint height) {
    bindRenderbuffer(renderbuffer);
    Renderbuffer::setupStorage(ALGINE_DEPTH_COMPONENT, width, height);
    Framebuffer::attachRenderbuffer(ALGINE_DEPTH_ATTACHMENT, renderbuffer);
    bindRenderbuffer(0);
}

void AlgineRenderer::mainPass(const uint displayFBO) {
    glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AlgineRenderer::bloomSearchPass(const uint bsFBO, const uint image) {
    bindFramebuffer(bsFBO);
    bloomSearchShader->use();
    texture2DAB(0, image);
    quadRenderer->drawQuad();
}

void AlgineRenderer::blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], const uint image, const uint blurAmount) {
    horizontal = true; 
	firstIteration = true;
    for (usize i = 0; i < blurAmount; i++) {
        blurShaders[horizontal]->use();

		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        
        texture2DAB(0, firstIteration ? image : pingpongBuffers[!horizontal]); // bloom
        
        // rendering
		quadRenderer->drawQuad();
		horizontal = !horizontal;
		if (firstIteration) firstIteration = false;
	}
}

void AlgineRenderer::screenspacePass(const uint ssFBO, const uint colorMap, const uint normalMap, const uint ssrValuesMap, const uint positionMap) {
    glBindFramebuffer(GL_FRAMEBUFFER, ssFBO);
    ssrShader->use();
    texture2DAB(0, colorMap);
    texture2DAB(1, normalMap);
    texture2DAB(2, ssrValuesMap);
    texture2DAB(3, positionMap);
    quadRenderer->drawQuad();
}

void AlgineRenderer::dofCoCPass(const uint cocFBO, const uint positionMap) {
    bindFramebuffer(cocFBO);
    dofCoCShader->use();
    texture2DAB(0, positionMap);
    quadRenderer->drawQuad();
}

#define _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, code_tex_ab) \
    horizontal = true;                                                 \
	firstIteration = true;                                             \
    for (size_t i = 0; i < blurAmount; i++) {                          \
        dofBlurShaders[horizontal]->use();                             \
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);    \
        code_tex_ab                                                    \
        /* rendering */                                                \
		quadRenderer->drawQuad();                                      \
		horizontal = !horizontal;                                      \
		if (firstIteration) firstIteration = false;                    \
	}

// dofMap may be position map or coc map depending on the method you use
void AlgineRenderer::dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint image, const uint dofMap, const uint blurAmount) {
    _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, {
        texture2DAB(0, firstIteration ? image : dofBuffers[!horizontal]);
        texture2DAB(1, dofMap);
    })
}

void AlgineRenderer::dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint image, const uint cocMap, const uint positionMap, const uint blurAmount) {
    _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, {
        texture2DAB(0, firstIteration ? image : dofBuffers[!horizontal]);
        texture2DAB(1, cocMap);
        texture2DAB(2, positionMap);
    })
}

#undef _dofBlurPass

void AlgineRenderer::doubleBlendPass(const uint image, const uint bloom) {
    // Do not need to call glClear, because the texture is completely redrawn
    blendShader->use();
    texture2DAB(0, image);
	texture2DAB(1, bloom);
	quadRenderer->drawQuad();
}

void AlgineRenderer::blendPass(const uint texture0) {
    // Do not need to call glClear, because the texture is completely redrawn
    blendShader->use();
    texture2DAB(0, texture0);
	quadRenderer->drawQuad();
}

AlgineRenderer::~AlgineRenderer() {
    #ifdef ALGINE_LOGGING
    std::cout << "~AlgineRenderer() " << this << "\n";
    #endif
}

}

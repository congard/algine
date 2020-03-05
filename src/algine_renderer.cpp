#include <algine/algine_renderer.h>

#include <GL/glew.h>
#include <algine/texture.h>
#include <algine/framebuffer.h>

namespace algine {
void AlgineRenderer::dofCoCPass(const uint cocFBO, const uint positionMap) {
    bindFramebuffer(cocFBO);
    dofCoCShader->use();
    texture2DAB(0, positionMap);
    quadRenderer->draw();
}

#define _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, code_tex_ab) \
    horizontal = true;                                                 \
	firstIteration = true;                                             \
    for (size_t i = 0; i < blurAmount; i++) {                          \
        dofBlurShaders[horizontal]->use();                             \
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);    \
        code_tex_ab                                                    \
        /* rendering */                                                \
		quadRenderer->draw();                                          \
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
	quadRenderer->draw();
}

void AlgineRenderer::blendPass(const uint texture0) {
    // Do not need to call glClear, because the texture is completely redrawn
    blendShader->use();
    texture2DAB(0, texture0);
	quadRenderer->draw();
}
}

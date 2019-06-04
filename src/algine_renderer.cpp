#ifndef ALGINE_RENDERER_CPP
#define ALGINE_RENDERER_CPP

#include <iostream>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "algine_structs.cpp"
#include "texture.cpp"
#include "framebuffer.cpp"
#include "renderbuffer.cpp"
#include "shaderprogram.cpp"
#include <algine/types.h>
#include <algine/constants.h>

namespace algine {
inline void pointer(GLint location, int count, GLuint buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        location, // attribute location
        count,    // count (1, 2, 3 or 4)
        GL_FLOAT, // type
        GL_FALSE, // is normalized?
        0,        // step
        nullptr   // offset
    );
}

inline void pointerui(GLint location, int count, GLuint buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribIPointer(
        location, // attribute location
        count,    // count (1, 2, 3 or 4)
        GL_UNSIGNED_INT, // type
        0,        // step
        nullptr   // offset
    );
}

// GLuint location, glm::vec3 vec
#define setVec3(location, vec) glUniform3f(location, vec.x, vec.y, vec.z)
// GLuint location, glm::mat4 mat
#define setMat4(location, mat) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))

struct AlgineRenderer {
    SSRShader *ssrs;
    BloomSearchShader *bloomSearchShader;
    BlurShader *blurShaders[2]; // { horizontal, vertical }
    BlendShader *blendShader;
    
    GLuint quadBuffers[2]; // vertexBuffer and texCoordsBuffer
    DOFBlurShader *dofBlurShaders[2]; // { horizontal, vertical }

    float gamma = 1;
    float exposure = 3;

    bool horizontal = true, firstIteration = true; // blur variables

    void renderQuad(const int &inPosLocation, const int &inTexCoordLocation) {
		glEnableVertexAttribArray(inPosLocation);
        glEnableVertexAttribArray(inTexCoordLocation);
	    pointer(inPosLocation, 3, quadBuffers[0]);
		pointer(inTexCoordLocation, 2, quadBuffers[1]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
		glDisableVertexAttribArray(inPosLocation);
        glDisableVertexAttribArray(inTexCoordLocation);
	}

    // configures renderbuffer for main pass rendering
    // note: this function will bind empty renderbuffer (`bindRenderbuffer(0)`)
    void configureMainPassRenderbuffer(const uint &renderbuffer, const uint &width, const uint &height) {
        bindRenderbuffer(renderbuffer);
        Renderbuffer::setupStorage(ALGINE_DEPTH_COMPONENT, width, height);
        Framebuffer::attachRenderbuffer(ALGINE_DEPTH_ATTACHMENT, renderbuffer);
        bindRenderbuffer(0);
    }

    void mainPass(const uint &displayFBO, const uint &rboBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void bloomSearchPass(const uint &bsFBO, const uint &image) {
        bindFramebuffer(bsFBO);
        useShaderProgram(bloomSearchShader->programId);
        texture2DAB(0, image);
        renderQuad(bloomSearchShader->inPosition, bloomSearchShader->inTexCoord);
    }

    void blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], const uint &image, const uint &blurAmount) {
        horizontal = true; 
		firstIteration = true;
        for (usize i = 0; i < blurAmount; i++) {
            glUseProgram(blurShaders[horizontal]->programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            
            texture2DAB(0, firstIteration ? image : pingpongBuffers[!horizontal]); // bloom
            
            // rendering
			renderQuad(blurShaders[horizontal]->inPosition, blurShaders[horizontal]->inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void screenspacePass(uint ssFBO, uint colorMap, uint normalMap, uint ssrValuesMap, uint positionMap) {
        glBindFramebuffer(GL_FRAMEBUFFER, ssFBO);
        glUseProgram(ssrs->programId);
        texture2DAB(0, colorMap);
        texture2DAB(1, normalMap);
        texture2DAB(2, ssrValuesMap);
        texture2DAB(3, positionMap);
        renderQuad(ssrs->inPosition, ssrs->inTexCoord);
    }

    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint &dofMap, const uint &sceneMap, const uint &blurAmount) {
        horizontal = true; 
		firstIteration = true;
        for (size_t i = 0; i < blurAmount; i++) {
            glUseProgram(dofBlurShaders[horizontal]->programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            texture2DAB(0, firstIteration ? sceneMap : dofBuffers[!horizontal]);
            texture2DAB(1, dofMap);

            // rendering
			renderQuad(dofBlurShaders[horizontal]->inPosition, dofBlurShaders[horizontal]->inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void doubleBlendPass(const uint &image, const uint &bloom) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, image);
		texture2DAB(1, bloom);
		renderQuad(blendShader->inPosition, blendShader->inTexCoord);
    }

    void blendPass(const uint &texture0) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, texture0);
		renderQuad(blendShader->inPosition, blendShader->inTexCoord);
    }

    void prepare() {
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

        // blend setting
        glUseProgram(blendShader->programId);
        glUniform1i(blendShader->samplerImage, 0);   // GL_TEXTURE0
        glUniform1i(blendShader->samplerBloom, 1); // GL_TEXTURE1
        glUniform1f(blendShader->exposure, exposure);
        glUniform1f(blendShader->gamma, gamma);

        // blur setting
        for (size_t i = 0; i < 2; i++) {
            glUseProgram(dofBlurShaders[i]->programId);
            glUniform1i(dofBlurShaders[i]->samplerImage, 0);
            glUniform1i(dofBlurShaders[i]->samplerDofBuffer, 1);
        }

        // screen space setting
        glUseProgram(ssrs->programId);
        glUniform1i(ssrs->samplerNormalMap, 1);
        glUniform1i(ssrs->samplerSSRValuesMap, 2);
        glUniform1i(ssrs->samplerPositionMap, 3);
        glUseProgram(0);
    }

    ~AlgineRenderer() {
        glDeleteBuffers(2, quadBuffers);
        #ifdef ALGINE_LOGGING
        std::cout << "~AlgineRenderer() " << this << "\n";
        #endif
    }
};

}

#endif /* ALGINE_RENDERER_CPP */

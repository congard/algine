#ifndef ALGINE_RENDERER_CPP
#define ALGINE_RENDERER_CPP

#include <iostream>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "algine_structs.cpp"
#include "texture.cpp"
#include <algine/framebuffer.h>
#include <algine/renderbuffer.h>
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

struct CubeRenderer {
    uint cubeBuffer;

    void init() {
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
    }

    inline void render(const int &inPosLocation) {
        glEnableVertexAttribArray(inPosLocation);
	    pointer(inPosLocation, 3, cubeBuffer);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);

        glDisableVertexAttribArray(inPosLocation);
    }

    inline void render(const int &programId, const int &inPosLocation) {
        useShaderProgram(programId);
        render(inPosLocation);
    }

    ~CubeRenderer() {
        glDeleteBuffers(1, &cubeBuffer);
    }
};

struct QuadRenderer {
    uint quadBuffers[2]; // vertexBuffer and texCoordsBuffer

    void init() {
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
    }

    void render(const int &inPosLocation, const int &inTexCoordLocation) {
		glEnableVertexAttribArray(inPosLocation);
        glEnableVertexAttribArray(inTexCoordLocation);
	    pointer(inPosLocation, 3, quadBuffers[0]);
		pointer(inTexCoordLocation, 2, quadBuffers[1]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
		glDisableVertexAttribArray(inPosLocation);
        glDisableVertexAttribArray(inTexCoordLocation);
	}

    void render(const int &programId, const int &inPosLocation, const int &inTexCoordLocation) {
        useShaderProgram(programId);
        render(inPosLocation, inTexCoordLocation);
    }

    ~QuadRenderer() {
        glDeleteBuffers(2, quadBuffers);
    }
};

struct AlgineRenderer {
    SSRShader *ssrs;
    BloomSearchShader *bloomSearchShader;
    BlurShader *blurShaders[2]; // { horizontal, vertical }
    BlendShader *blendShader;
    DOFBlurShader *dofBlurShaders[2]; // { horizontal, vertical }
    DOFCoCShader *dofCoCShader;

    QuadRenderer *quadRenderer;

    bool horizontal = true, firstIteration = true; // blur variables

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
        quadRenderer->render(bloomSearchShader->inPosition, bloomSearchShader->inTexCoord);
    }

    void blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], const uint &image, const uint &blurAmount) {
        horizontal = true; 
		firstIteration = true;
        for (usize i = 0; i < blurAmount; i++) {
            glUseProgram(blurShaders[horizontal]->programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            
            texture2DAB(0, firstIteration ? image : pingpongBuffers[!horizontal]); // bloom
            
            // rendering
			quadRenderer->render(blurShaders[horizontal]->inPosition, blurShaders[horizontal]->inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void screenspacePass(const uint &ssFBO, const uint &colorMap, const uint &normalMap, const uint &ssrValuesMap, const uint &positionMap) {
        glBindFramebuffer(GL_FRAMEBUFFER, ssFBO);
        glUseProgram(ssrs->programId);
        texture2DAB(0, colorMap);
        texture2DAB(1, normalMap);
        texture2DAB(2, ssrValuesMap);
        texture2DAB(3, positionMap);
        quadRenderer->render(ssrs->inPosition, ssrs->inTexCoord);
    }

    void dofCoCPass(const uint &cocFBO, const uint &positionMap) {
        bindFramebuffer(cocFBO);
        useShaderProgram(dofCoCShader->programId);
        texture2DAB(0, positionMap);
        quadRenderer->render(dofCoCShader->inPosition, dofCoCShader->inTexCoord);
    }

    #define _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, code_tex_ab) \
        horizontal = true;                                                 \
		firstIteration = true;                                             \
        for (size_t i = 0; i < blurAmount; i++) {                          \
            glUseProgram(dofBlurShaders[horizontal]->programId);           \
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);    \
            code_tex_ab                                                    \
            /* rendering */                                                \
			quadRenderer->render(dofBlurShaders[horizontal]->inPosition, dofBlurShaders[horizontal]->inTexCoord); \
			horizontal = !horizontal;                                      \
			if (firstIteration) firstIteration = false;                    \
		}

    // dofMap may be position map or coc map depending on the method you use
    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint &dofMap, const uint &image, const uint &blurAmount) {
        _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, {
            texture2DAB(0, firstIteration ? image : dofBuffers[!horizontal]);
            texture2DAB(1, dofMap);
        })
    }

    #undef _dofBlurPass

    void doubleBlendPass(const uint &image, const uint &bloom) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, image);
		texture2DAB(1, bloom);
		quadRenderer->render(blendShader->inPosition, blendShader->inTexCoord);
    }

    void blendPass(const uint &texture0) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, texture0);
		quadRenderer->render(blendShader->inPosition, blendShader->inTexCoord);
    }

    ~AlgineRenderer() {
        #ifdef ALGINE_LOGGING
        std::cout << "~AlgineRenderer() " << this << "\n";
        #endif
    }
};

}

#endif /* ALGINE_RENDERER_CPP */

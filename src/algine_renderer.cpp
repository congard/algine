#ifndef ALGINE_RENDERER_CPP
#define ALGINE_RENDERER_CPP

#include <iostream>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algine/algine_structs.h>
#include <algine/texture.h>
#include <algine/framebuffer.h>
#include <algine/renderbuffer.h>
#include <algine/shader_program.h>
#include <algine/types.h>
#include <algine/constants.h>

namespace algine {
inline void pointer(const int location, const int count, const uint buffer, const uint stride = 0, const void *offset = nullptr) {
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

inline void pointerui(const int location, const int count, const uint buffer, const uint stride = 0, const void *offset = nullptr) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribIPointer(
        location,        // attribute location
        count,           // count (1, 2, 3 or 4)
        GL_UNSIGNED_INT, // type
        stride,          // step
        offset           // offset
    );
}

// GLuint location, glm::vec3 vec
#define setVec3(location, vec) glUniform3f(location, vec.x, vec.y, vec.z)
// GLuint location, glm::mat4 mat
#define setMat4(location, mat) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))

struct CubeRenderer {
    uint cubeBuffer, cubeVAO;

    // if `inPosLocation` != -1, VAO will be created
    void init(const int &inPosLocation = -1) {
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

    inline void bindVAO() {
        glBindVertexArray(cubeVAO);
    }

    // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 14)`
    inline void drawCube() {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    }

    inline void render() {
        bindVAO();
        drawCube();
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
        glDeleteVertexArrays(1, &cubeVAO);
    }
};

struct QuadRenderer {
    uint quadBuffers[2]; // vertexBuffer and texCoordsBuffer
    uint quadVAO;

    void init(const int &inPosLocation = -1, const int &inTexCoordLocation = -1) {
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

    inline void bindVAO() {
        glBindVertexArray(quadVAO);
    }

    // just calls `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)`
    inline void drawQuad() {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    inline void render() {
        bindVAO();
        drawQuad();
    }

    inline void render(const int &inPosLocation, const int &inTexCoordLocation) {
		glEnableVertexAttribArray(inPosLocation);
        glEnableVertexAttribArray(inTexCoordLocation);
	    pointer(inPosLocation, 3, quadBuffers[0]);
		pointer(inTexCoordLocation, 2, quadBuffers[1]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
		glDisableVertexAttribArray(inPosLocation);
        glDisableVertexAttribArray(inTexCoordLocation);
	}

    inline void render(const int &programId, const int &inPosLocation, const int &inTexCoordLocation) {
        useShaderProgram(programId);
        render(inPosLocation, inTexCoordLocation);
    }

    ~QuadRenderer() {
        glDeleteBuffers(2, quadBuffers);
        glDeleteVertexArrays(1, &quadVAO);
    }
};

struct AlgineRenderer {
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
        quadRenderer->drawQuad();
    }

    void blurPass(const uint pingpongFBO[2], const uint pingpongBuffers[2], const uint &image, const uint &blurAmount) {
        horizontal = true; 
		firstIteration = true;
        for (usize i = 0; i < blurAmount; i++) {
            glUseProgram(blurShaders[horizontal]->programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            
            texture2DAB(0, firstIteration ? image : pingpongBuffers[!horizontal]); // bloom
            
            // rendering
			quadRenderer->drawQuad();
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
        quadRenderer->drawQuad();
    }

    void dofCoCPass(const uint &cocFBO, const uint &positionMap) {
        bindFramebuffer(cocFBO);
        useShaderProgram(dofCoCShader->programId);
        texture2DAB(0, positionMap);
        quadRenderer->drawQuad();
    }

    #define _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, code_tex_ab) \
        horizontal = true;                                                 \
		firstIteration = true;                                             \
        for (size_t i = 0; i < blurAmount; i++) {                          \
            glUseProgram(dofBlurShaders[horizontal]->programId);           \
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);    \
            code_tex_ab                                                    \
            /* rendering */                                                \
			quadRenderer->drawQuad();                                      \
			horizontal = !horizontal;                                      \
			if (firstIteration) firstIteration = false;                    \
		}

    // dofMap may be position map or coc map depending on the method you use
    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint &image, const uint &dofMap, const uint &blurAmount) {
        _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, {
            texture2DAB(0, firstIteration ? image : dofBuffers[!horizontal]);
            texture2DAB(1, dofMap);
        })
    }

    void dofBlurPass(const uint pingpongFBO[2], const uint dofBuffers[2], const uint &image, const uint &cocMap, const uint &positionMap, const uint &blurAmount) {
        _dofBlurPass(pingpongFBO, dofBuffers, blurAmount, {
            texture2DAB(0, firstIteration ? image : dofBuffers[!horizontal]);
            texture2DAB(1, cocMap);
            texture2DAB(2, positionMap);
        })
    }

    #undef _dofBlurPass

    void doubleBlendPass(const uint &image, const uint &bloom) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, image);
		texture2DAB(1, bloom);
		quadRenderer->drawQuad();
    }

    void blendPass(const uint &texture0) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(blendShader->programId);
        texture2DAB(0, texture0);
		quadRenderer->drawQuad();
    }

    ~AlgineRenderer() {
        #ifdef ALGINE_LOGGING
        std::cout << "~AlgineRenderer() " << this << "\n";
        #endif
    }
};

}

#endif /* ALGINE_RENDERER_CPP */

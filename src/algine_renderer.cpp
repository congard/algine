#ifndef ALGINE_RENDERER_CPP
#define ALGINE_RENDERER_CPP

#include <iostream>

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "algine_structs.cpp"
#include "texture.cpp"

namespace algine {
static const GLfloat ALGINE_RED[4] = { 1.0, 0.0, 0.0, 0.0 };

#define clearDOFBuffer glClearBufferfv(GL_COLOR, 1, ALGINE_RED);

inline void pointer(int location, int count, GLuint buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        location, // attribute location
        count,    // count (2 or 3)
        GL_FLOAT, // type
        GL_FALSE, // is normalized?
        0,        // step
       (void *)0  // offset
    );
}

// GLuint location, glm::vec3 vec
#define setVec3(location, vec) glUniform3f(location, vec.x, vec.y, vec.z)
// GLuint location, glm::mat4 mat
#define setMat4(location, mat) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))

// main, god rays, dof (float), depth (float), normal (vec3), reflection strength (float), position (vec3, in world space)

struct AlgineRenderer {
    AlginePrograms programs;
    AlgineParams *params;

    GLuint
        quadBuffers[2]; // vertexBuffer and texCoordsBuffer

    struct BlusProgram {
        GLuint
            programId,
            inPos,
            inTexCoord,
            samplerDof,
            samplerDofBuffer;
    };

    BlusProgram blusPrograms[2];

    bool horizontal = true, firstIteration = true; // blur variables

	void renderQuad(const GLuint &inPosLocation, const GLuint &inTexCoordLocation) {
		glEnableVertexAttribArray(inPosLocation);
        glEnableVertexAttribArray(inTexCoordLocation);
	    pointer(inPosLocation, 3, quadBuffers[0]);
		pointer(inTexCoordLocation, 2, quadBuffers[1]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
		glDisableVertexAttribArray(inPosLocation);
        glDisableVertexAttribArray(inTexCoordLocation);
	}

    void mainPass(GLuint displayFBO, GLuint rboBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, rboBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, params->SCR_W, params->SCR_H);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void screenspacePass(GLuint ssFBO, GLuint colorMap, GLuint normalMap, GLuint ssrValuesMap, GLuint positionMap) {
        glBindFramebuffer(GL_FRAMEBUFFER, ssFBO);
        // developer must do this before call screenspacePass
        // glBindTexture(GL_TEXTURE_2D, ssBuffer);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, params.SCR_W, params.SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
        // glBindTexture(GL_TEXTURE_2D, bloomBuffer);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, params.SCR_W, params.SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
        glUseProgram(programs.PROGRAM_ID_SSS);
        texture2DAB(0, colorMap);
        texture2DAB(1, normalMap);
        texture2DAB(2, ssrValuesMap);
        texture2DAB(3, positionMap);
        renderQuad(programs.SSS_IN_POSITION, programs.SSS_IN_TEXCOORD);
    }

    void bloomDofPass(const GLuint pingpongFBO[2], const GLuint pingpongBuffers[2], const GLuint dofBuffers[2], 
                      const GLuint bloomMap, const GLuint dofMap, const GLuint sceneMap) {
        // developer must do this before call bloomDofPass
        // // configuring textures
		// for (int i = 0; i < 2; i++) {
        //     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);

        //     // bloom
		// 	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGB, GL_FLOAT, NULL);

        //     // dof
		// 	glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
		// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
		// }
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < params->BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            texture2DAB(0, firstIteration ? bloomMap : pingpongBuffers[!horizontal]); // bloom
            texture2DAB(1, firstIteration ? sceneMap : dofBuffers[!horizontal]); // dof
            texture2DAB(2, dofMap);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void bloomPass(const GLuint pingpongFBO[2], const GLuint pingpongBuffers[2], 
                   const GLuint bloomMap) {
        // developer must do this before call bloomPass
        // // configuring textures
        // glActiveTexture(GL_TEXTURE0);
		// for (int i = 0; i < 2; i++) {
        //     // bloom
		// 	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		// 	glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGB, GL_FLOAT, NULL);
		// }
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < params->BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            // bloom
			glBindTexture(GL_TEXTURE_2D, firstIteration ? bloomMap : pingpongBuffers[!horizontal]);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void dofPass(const GLuint pingpongFBO[2], const GLuint dofBuffers[2], 
                 const GLuint dofMap, const GLuint sceneMap) {
        // // configuring textures
        // glActiveTexture(GL_TEXTURE0);
		// for (int i = 0; i < 2; i++) {
        //     // dof
		// 	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		// 	glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
		// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ALGINE_SCR_W, ALGINE_SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
		// }
			
		horizontal = true; 
		firstIteration = true;
		for (int i = 0; i < params->BLUR_AMOUNT; i++) {
            glUseProgram(blusPrograms[horizontal].programId);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

            // dof
            texture2DAB(1, firstIteration ? sceneMap : dofBuffers[!horizontal]);
            texture2DAB(2, dofMap);

            // rendering
			renderQuad(blusPrograms[horizontal].inPos, blusPrograms[horizontal].inTexCoord);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}
    }

    void doubleBlendPass(const GLuint texture0, const GLuint texture1) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(programs.PROGRAM_ID_BLES);
        texture2DAB(0, texture0);
		texture2DAB(1, texture1);
		renderQuad(programs.BLES_IN_POSITION, programs.BLES_IN_TEXCOORD);
    }

    void blendPass(const GLuint texture0) {
        // Do not need to call glClear, because the texture is completely redrawn
		glUseProgram(programs.PROGRAM_ID_BLES);
        texture2DAB(0, texture0);
		renderQuad(programs.BLES_IN_POSITION, programs.BLES_IN_TEXCOORD);
    }

    void prepare() {
        blusPrograms[0] = BlusProgram {
            programs.PROGRAM_ID_BLUS_VERTICAL, programs.BLUS_V_IN_POSITION, programs.BLUS_V_IN_TEXCOORD, programs.BLUS_V_SAMPLER_SCENE, programs.BLUS_V_SAMPLER_DOF_BUFFER
        };

        blusPrograms[1] = BlusProgram { 
            programs.PROGRAM_ID_BLUS_HORIZONTAL, programs.BLUS_H_IN_POSITION, programs.BLUS_H_IN_TEXCOORD, programs.BLUS_H_SAMPLER_SCENE, programs.BLUS_H_SAMPLER_DOF_BUFFER
        };

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
        glUseProgram(programs.PROGRAM_ID_BLES);
        glUniform1i(programs.BLES_SAMPLER_BLOOM_SCENE, 1); // GL_TEXTURE1
        glUniform1i(programs.BLES_SAMPLER_DOF_SCENE, 0);   // GL_TEXTURE0
        glUniform1f(programs.BLES_EXPOSURE, params->EXPOSURE);
        glUniform1f(programs.BLES_GAMMA, params->GAMMA);

        // blur setting
        for (size_t i = 0; i < 2; i++) {
            glUseProgram(blusPrograms[i].programId);
            glUniform1i(blusPrograms[i].samplerDof, 1);
            glUniform1i(blusPrograms[i].samplerDofBuffer, 2);
        }

        // screen space setting
        glUseProgram(programs.PROGRAM_ID_SSS);
        glUniform1i(programs.SSS_SAMPLER_MAP_NORMAL, 1);
        glUniform1i(programs.SSS_SAMPLER_MAP_SSRVALUES, 2);
        glUniform1i(programs.SSS_SAMPLER_MAP_POSITION, 3);
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
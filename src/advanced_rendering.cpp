/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_ADVANCED_RENDERING_CPP
#define ALGINE_ADVANCED_RENDERING_CPP

#include <GL/glew.h>

#include <bits/basic_string.h>

#include "utils/shader_utils.cpp"
#include "utils/math.cpp"

#define BLOOM_KERNEL_SIZE 15
#define BLOOM_KERNEL_SIGMA 6.0f

namespace advren {
    GLuint blurAmount = 4, SCR_W, SCR_H;
    float exposure = 3.0f, gamma = 1.0f; // blend variables
    
	// anonymous namespace
	namespace {
		ShaderProgram shaderBlurProgram, shaderBlendProgram;
    	GLuint displayFBO, 
			pingpongFBO[2], colorBuffers[2], 
			pingpongBuffers[2], // blur buffers
			dofBuffers[2], // dof buffers
        	rboBuffer, 
        	quadBuffers[2], // vertexBuffer and texCoordsBuffer
        	colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		bool horizontal = true, firstIteration = true; // blur variables

		void applyDefaultTextureParams() {
        	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	}

		void renderQuad(ShaderProgram &shaderProgram) {
			glEnableVertexAttribArray(shaderProgram.getValueId("inPos"));
        	glEnableVertexAttribArray(shaderProgram.getValueId("inTexCoords"));
	    	su::pointer(shaderProgram.getValueId("inPos"), 3, quadBuffers[0]);
			su::pointer(shaderProgram.getValueId("inTexCoords"), 2, quadBuffers[1]);
		
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			glDisableVertexAttribArray(shaderProgram.getValueId("inPos"));
        	glDisableVertexAttribArray(shaderProgram.getValueId("inTexCoords"));
		}

    	void mkBlur() {
			// configuring textures
			for (int i = 0; i < 2; i++) {
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
				glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_W, SCR_H, 0, GL_RGB, GL_FLOAT, NULL);

				glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_W, SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
				glClearColor(0, 0, 0, 1); // for effective dof
			}
			
			horizontal = true; 
			firstIteration = true;
			glUseProgram(shaderBlurProgram.programId);
			for (int i = 0; i < blurAmount; i++) {
				glActiveTexture(GL_TEXTURE0);
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				
				glUniform1i(shaderBlurProgram.getValueId("isHorizontal"), horizontal);
				glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);

				glActiveTexture(GL_TEXTURE1);
				glUniform1i(shaderBlurProgram.getValueId("image2"), 1);
				glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[0] : dofBuffers[!horizontal]);

				renderQuad(shaderBlurProgram);
				horizontal = !horizontal;
				if (firstIteration) firstIteration = false;
			}
		}

    	void blend() {
			// Do not need to call glClear, because the texture is completely redrawn
			glUseProgram(shaderBlendProgram.programId);
			// glActiveTexture(GL_TEXTURE0);
			// glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			// ^^^ will be available in 1.2 alpha ^^^
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, dofBuffers[!horizontal]);
			renderQuad(shaderBlendProgram);
		}
	}

    void init() {
        glGenFramebuffers(1, &displayFBO);
        glGenTextures(2, colorBuffers);
		glGenRenderbuffers(1, &rboBuffer);
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffers);
		glGenTextures(2, dofBuffers);

		// configuring display framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
		glDrawBuffers(2, colorAttachments); // use 2 color components

        // configuring textures
		for (int i = 0; i < 2; i++) {
			// configuring render textures
			glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			applyDefaultTextureParams();
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
				
			// configuring ping-pong (blur) textures
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glDrawBuffers(2, colorAttachments); // use 2 color components

			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
			applyDefaultTextureParams();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, dofBuffers[i]);
			applyDefaultTextureParams();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dofBuffers[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // loading blur shaders
        shaderBlurProgram.init("src/resources/shaders/vertex_blur_shader.glsl", "src/resources/shaders/fragment_blur_shader.glsl");
		shaderBlurProgram.loadValueId("inPos", GLSL_ATTRIBUTE);
		shaderBlurProgram.loadValueId("inTexCoords", GLSL_ATTRIBUTE);
		shaderBlurProgram.loadValueId("image", GLSL_UNIFORM); // bloom
		shaderBlurProgram.loadValueId("image2", GLSL_UNIFORM); // dof
		shaderBlurProgram.loadValueId("isHorizontal", GLSL_UNIFORM);
		shaderBlurProgram.loadValueId("kernel", GLSL_UNIFORM);
		shaderBlurProgram.loadValueId("max_sigma", GLSL_UNIFORM);
		shaderBlurProgram.loadValueId("min_sigma", GLSL_UNIFORM);

        // configuring blur kernel
		// sending to shader center of kernel and right part
		float kernel[BLOOM_KERNEL_SIZE];
		getGB1DKernel(kernel, BLOOM_KERNEL_SIZE, BLOOM_KERNEL_SIGMA);
		glUseProgram(shaderBlurProgram.programId);
		for (int i = 0; i < BLOOM_KERNEL_SIZE / 2 + 1; i++) 
            glUniform1f(
                glGetUniformLocation(shaderBlurProgram.programId, ("kernel[" + std::to_string(i) + "]").c_str()), 
                kernel[BLOOM_KERNEL_SIZE / 2 + i]
            );
		glUseProgram(0);

        // creating buffers for quad rendering
        static float vertices[12] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        }, texCoords[8] = {
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

        // loading blend shaders
        shaderBlendProgram.init("src/resources/shaders/vertex_blend_shader.glsl", "src/resources/shaders/fragment_blend_shader.glsl");
        shaderBlendProgram.loadValueId("inPos", GLSL_ATTRIBUTE);
		shaderBlendProgram.loadValueId("inTexCoords", GLSL_ATTRIBUTE);
		// shaderBlendProgram.loadValueId("scene", GLSL_UNIFORM); // will be available in 1.2 alpha
		shaderBlendProgram.loadValueId("bluredScene", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("dofScene", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("exposure", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("gamma", GLSL_UNIFORM);

        // blend setting
        glUseProgram(shaderBlendProgram.programId);
        // glUniform1i(shaderBlendProgram.getValueId("scene"), 0); // GL_TEXTURE0 // will be available in 1.2 alpha
        glUniform1i(shaderBlendProgram.getValueId("bluredScene"), 1); // GL_TEXTURE1
		glUniform1i(shaderBlendProgram.getValueId("dofScene"), 2); // GL_TEXTURE2
        glUniform1f(shaderBlendProgram.getValueId("exposure"), exposure);
        glUniform1f(shaderBlendProgram.getValueId("gamma"), gamma);
        glUseProgram(0);
    }

    void begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, rboBuffer);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_W, SCR_H);
	    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboBuffer);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear it
	    // configuring textures
		for (int i = 0; i < 2; i++) {
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_W, SCR_H, 0, GL_RGBA, GL_FLOAT, NULL);
		}
    }

    void end() {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		mkBlur();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		blend();
		glUseProgram(0);
	}

	void recycle() {
        glDeleteFramebuffers(1, &displayFBO);
        glDeleteTextures(2, colorBuffers);
		glDeleteRenderbuffers(1, &rboBuffer);
		glDeleteFramebuffers(2, pingpongFBO);
		glDeleteTextures(2, pingpongBuffers);
		glDeleteTextures(2, dofBuffers);
		glDeleteBuffers(2, quadBuffers);

		#ifdef ALGINE_LOGGING
        std::cout << "~AdvancedRendering\n";
        #endif
    }
};

#undef BLOOM_KERNEL_SIZE
#undef BLOOM_KERNEL_SIGMA

#endif /* ALGINE_ADVANCED_RENDERING_CPP */
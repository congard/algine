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

#define BLUR_KERNEL_SIZE 15
#define BLUR_KERNEL_SIGMA 6.0f

namespace advren {
    GLuint blurAmount = 4, SCR_W, SCR_H;
    float exposure = 3.0f, gamma = 1.0f; // blend variables
    std::string blur_kernel_name = "kernel";

	// anonymous namespace
	namespace {
		ShaderProgram shaderBlurProgram, shaderBlendProgram;
    	GLuint displayFBO, 
			pingpongFBO[2], colorBuffers[2], pingpongBuffers[2], // blur
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
			glActiveTexture(GL_TEXTURE0);
			for (int i = 0; i < 2; i++) {
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
				glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_W, SCR_H, 0, GL_RGB, GL_FLOAT, NULL);
				// Do not need to call glClear, because the texture is completely redrawn
			}
			
			horizontal = true; 
			firstIteration = true;
			glUseProgram(shaderBlurProgram.programId);
			glActiveTexture(GL_TEXTURE0);
			for (int i = 0; i < blurAmount; i++) {
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				glUniform1i(shaderBlurProgram.getValueId("isHorizontal"), horizontal);
				glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffers[!horizontal]);
				renderQuad(shaderBlurProgram);
				horizontal = !horizontal;
				if (firstIteration) firstIteration = false;
			}
		}

    	void blend() {
			// Do not need to call glClear, because the texture is completely redrawn
			glUseProgram(shaderBlendProgram.programId);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
			renderQuad(shaderBlendProgram);
		}
	}

    void init() {
        glGenFramebuffers(1, &displayFBO);
        glGenTextures(2, colorBuffers);
		glGenRenderbuffers(1, &rboBuffer);
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffers);

        // configuring textures
		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < 2; i++) {
			// configuring render textures
			glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			applyDefaultTextureParams();
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
				
			// configuring ping-pong (blur) textures
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
			applyDefaultTextureParams();
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // loading blur shaders
        shaderBlurProgram.init("src/resources/shaders/vertex_blur_shader.glsl", "src/resources/shaders/fragment_blur_shader.glsl");
		shaderBlurProgram.loadValueId("inPos", GLSL_ATTRIBUTE);
		shaderBlurProgram.loadValueId("inTexCoords", GLSL_ATTRIBUTE);
		shaderBlurProgram.loadValueId("image", GLSL_UNIFORM);
		shaderBlurProgram.loadValueId("isHorizontal", GLSL_UNIFORM);
		shaderBlurProgram.loadValueId("kernel", GLSL_UNIFORM);

        // configuring blur kernel
		// sending to shader center of kernel and right part
		float kernel[BLUR_KERNEL_SIZE];
		getGB1DKernel(kernel, BLUR_KERNEL_SIZE, BLUR_KERNEL_SIGMA);
		glUseProgram(shaderBlurProgram.programId);
		for (int i = 0; i < BLUR_KERNEL_SIZE / 2 + 1; i++) 
            glUniform1f(
                glGetUniformLocation(shaderBlurProgram.programId, (blur_kernel_name + "[" + std::to_string(i) + "]").c_str()), 
                kernel[BLUR_KERNEL_SIZE / 2 + i]
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
		shaderBlendProgram.loadValueId("scene", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("bluredScene", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("exposure", GLSL_UNIFORM);
		shaderBlendProgram.loadValueId("gamma", GLSL_UNIFORM);

        // blend setting
        glUseProgram(shaderBlendProgram.programId);
        glUniform1i(shaderBlendProgram.getValueId("scene"), 0); // GL_TEXTURE0
        glUniform1i(shaderBlendProgram.getValueId("bluredScene"), 1); // GL_TEXTURE1
        glUniform1f(shaderBlendProgram.getValueId("exposure"), exposure);
        glUniform1f(shaderBlendProgram.getValueId("gamma"), gamma);
        glUseProgram(0);
    }

    void begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, displayFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, rboBuffer);
	    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_W, SCR_H);
	    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboBuffer);
	    glDrawBuffers(2, colorAttachments); // use 2 color components
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear it
	    // configuring textures
		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < 2; i++) {
			glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_W, SCR_H, 0, GL_RGB, GL_FLOAT, NULL);
		}
    }

    void end() {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glDrawBuffers(1, colorAttachments); // use 1 color component
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

		#ifdef ALGINE_LOGGING
        std::cout << "~AdvancedRendering\n";
        #endif
    }
};

#undef BLUR_KERNEL_SIZE
#undef BLUR_KERNEL_SIGMA

#endif /* ALGINE_ADVANCED_RENDERING_CPP */
/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#ifndef ALGINE_LAMP_CPP
#define ALGINE_LAMP_CPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

#include "algine_renderer.cpp"
#include "model.cpp"
#include <algine/types.h>
#include "algine_structs.cpp"
#include "framebuffer.cpp"
#include "texture.cpp"

namespace algine {
struct Light {
    glm::vec3 pos, color;
    float kc; // constant term; usually kept at 1.0
    float kl; // linear term
    float kq; // // quadratic term
    CShader *cs = nullptr;
    SShader *ss = nullptr;
    usize index = 0;
    usize textureStartId = 6; // normal, reflectionStrength, jitter, ambient, diffuse, specular - last id is 5
    usize shadowMapWidth, shadowMapHeight;
    uint depthMapFBO, depthMap;
    glm::mat4 lightProjection;

    virtual void push_pos() { }
    virtual void push_color() { }
    virtual void push_kc() { }
    virtual void push_kl() { }
    virtual void push_kq() { }
    virtual void push_shadowMap() { }

    virtual void pushAll() {
        push_pos();
        push_color();
        push_kc();
        push_kl();
        push_kq();
        push_shadowMap();
    }

    virtual void updateMatrices() { }

    virtual void initShadowMapping(usize shadowMapWidth, usize shadowMapHeight) {
        this->shadowMapWidth = shadowMapWidth;
        this->shadowMapHeight = shadowMapHeight;
        createDepthMap();
        createDepthMapFBO();
        updateMatrices();
        configureDepthMap();
    }

    virtual void createDepthMapFBO() {
        Framebuffer::create(&depthMapFBO);
    }

    virtual void createDepthMap() {
        Texture::create(&depthMap);
    }

    virtual void configureDepthMap() { }

    virtual void begin() { }
		
	virtual void end() { }

    Light() { /* empty for arrays */ }

    ~Light() {
        Framebuffer::destroy(&depthMapFBO);
        Texture::destroy(&depthMap);
        #ifdef ALGINE_LOGGING
        std::cout << "~Lamp()\n";
        #endif
    }

    /* --- --- */
};

struct DirLight : public Light {
    glm::mat4 lightView, lightSpace;
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    float minBias = 0.005f, maxBias = 0.05f;

    virtual void push_pos() { setVec3(cs->dirLights[index].pos, pos); }
    virtual void push_color() { setVec3(cs->dirLights[index].color, color); }
    virtual void push_kc() { glUniform1f(cs->dirLights[index].kc, kc); }
    virtual void push_kl() { glUniform1f(cs->dirLights[index].kl, kl); }
    virtual void push_kq() { glUniform1f(cs->dirLights[index].kq, kq); }
    virtual void push_lightMatrix() { setMat4(cs->dirLights[index].lightMatrix, lightSpace); }
    virtual void push_minBias() { glUniform1f(cs->dirLights[index].minBias, minBias); }
    virtual void push_maxBias() { glUniform1f(cs->dirLights[index].maxBias, maxBias); }
    virtual void push_shadowMap() {
        glUniform1i(cs->dirLights[index].shadowMap, textureStartId + index);
		glActiveTexture(GL_TEXTURE0 + textureStartId + index);
		glBindTexture(GL_TEXTURE_2D, depthMap);
    }

    virtual void pushAll() {
        Light::pushAll();
        push_lightMatrix();
        push_minBias();
        push_maxBias();
    }

    virtual void orthoShadowMapping(float left, float right, float bottom, float top, float near = 1.0f, float far = 32.0f) {
        lightProjection = glm::ortho(left, right, bottom, top, near, far);
    }

    virtual void perspectiveShadowMapping(float fovy, float aspect, float near = 1.0f, float far = 32.0f) {
        lightProjection = glm::perspective(fovy, aspect, near, far);
    }

    virtual void viewMatrixLookAt() {
        lightView = glm::lookAt(pos, lookAt, upVector);
    }

    virtual void updateLightSpaceMatrix() {
        lightSpace = lightProjection * lightView;
    }

    virtual void updateMatrices() {
        viewMatrixLookAt();
        updateLightSpaceMatrix();
    }

    virtual void configureDepthMap() {
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

    virtual void begin() {
        bindFramebuffer(depthMapFBO);
        activeTexture(0);
		bindTexture2D(depthMap);
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    }

    virtual void end() {
        bindFramebuffer(0);
    }
};

struct PointLight : public Light {
    // far plane
    float far = 32.0f;
    float bias = 0.4f;
    glm::mat4 lightViews[6], lightSpaceMatrices[6];

    virtual void push_pos() { setVec3(cs->pointLights[index].pos, pos); }
    virtual void push_color() { setVec3(cs->pointLights[index].color, color); }
    virtual void push_kc() { glUniform1f(cs->pointLights[index].kc, kc); }
    virtual void push_kl() { glUniform1f(cs->pointLights[index].kl, kl); }
    virtual void push_kq() { glUniform1f(cs->pointLights[index].kq, kq); }
    virtual void push_far() { glUniform1f(cs->pointLights[index].far, far); }
    virtual void push_bias() { glUniform1f(cs->pointLights[index].bias, bias); }
    virtual void push_shadowMap() {
        glUniform1i(cs->pointLights[index].shadowMap, textureStartId + index);
		glActiveTexture(GL_TEXTURE0 + textureStartId + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    }

    virtual void pushAll() {
        Light::pushAll();
        push_far();
        push_bias();
    }

    virtual void updateMatrices() {
        lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
        lightViews[0] = glm::lookAt(pos, glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec3(0, -1, 0));
        lightViews[1] = glm::lookAt(pos, glm::vec3(pos.x - 1, pos.y, pos.z), glm::vec3(0, -1, 0));
        lightViews[2] = glm::lookAt(pos, glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec3(0, 0, 1));
        lightViews[3] = glm::lookAt(pos, glm::vec3(pos.x, pos.y - 1, pos.z), glm::vec3(0, 0, -1));
        lightViews[4] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec3(0, -1, 0));
        lightViews[5] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z - 1), glm::vec3(0, -1, 0));
        for (size_t i = 0; i < 6; i++) lightSpaceMatrices[i] = lightProjection * lightViews[i];
    }

    virtual void setLightPosSS() {
        setVec3(ss->lampPos, pos);
    }

    virtual void setShadowMatricesSS() {
        for (size_t i = 0; i < 6; i++) setMat4(ss->shadowMatrices + i, lightSpaceMatrices[i]);
    }

    virtual void configureDepthMap() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
		for (size_t i = 0; i < 6; i++) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    virtual void begin() {
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	}

    virtual void end() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

};

struct PointLamp : public PointLight {
    Model *mptr;

    void setPos(const glm::vec3 &pos) {
        Light::pos = pos;
        mptr->setPos(pos);
        mptr->transform();
    }
};

struct DirLamp : public DirLight {
    Model *mptr;

    void setPos(const glm::vec3 &pos) {
        Light::pos = pos;
        mptr->setPos(pos);
        mptr->transform();
    }
};

} // namespace algine

#endif /* ALGINE_LAMP_CPP */

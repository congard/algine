#define GLM_FORCE_CTOR_INIT
#include <algine/light.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <iostream>
#include <algine/algine_renderer.h>
#include <algine/framebuffer.h>
#include <algine/texture.h>

namespace algine {
// struct Light
void Light::push_pos() { }
void Light::push_color() { }
void Light::push_kc() { }
void Light::push_kl() { }
void Light::push_kq() { }
void Light::push_shadowMap() { }

void Light::pushAll() {
    push_pos();
    push_color();
    push_kc();
    push_kl();
    push_kq();
    push_shadowMap();
}

void Light::updateMatrices() { }

void Light::initShadowMapping(const usize shadowMapWidth, const usize shadowMapHeight) {
    this->shadowMapWidth = shadowMapWidth;
    this->shadowMapHeight = shadowMapHeight;
    createDepthMap();
    createDepthMapFBO();
    updateMatrices();
    configureDepthMap();
}

void Light::createDepthMapFBO() {
    Framebuffer::create(&depthMapFBO);
}

void Light::createDepthMap() {
    Texture::create(&depthMap);
}

void Light::configureDepthMap() { }

void Light::begin() { }
	
void Light::end() { }

Light::Light() { /* empty for arrays */ }

Light::~Light() {
    Framebuffer::destroy(&depthMapFBO);
    Texture::destroy(&depthMap);
    #ifdef ALGINE_LOGGING
    std::cout << "~Lamp()\n";
    #endif
}

// struct DirLight
void DirLight::push_pos() { setVec3(cs->dirLights[index].pos, pos); }
void DirLight::push_color() { setVec3(cs->dirLights[index].color, color); }
void DirLight::push_kc() { glUniform1f(cs->dirLights[index].kc, kc); }
void DirLight::push_kl() { glUniform1f(cs->dirLights[index].kl, kl); }
void DirLight::push_kq() { glUniform1f(cs->dirLights[index].kq, kq); }
void DirLight::push_lightMatrix() { setMat4(cs->dirLights[index].lightMatrix, lightSpace); }
void DirLight::push_minBias() { glUniform1f(cs->dirLights[index].minBias, minBias); }
void DirLight::push_maxBias() { glUniform1f(cs->dirLights[index].maxBias, maxBias); }
void DirLight::push_shadowMap() {
    glUniform1i(cs->dirLights[index].shadowMap, textureStartId + index);
	glActiveTexture(GL_TEXTURE0 + textureStartId + index);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

void DirLight::pushAll() {
    Light::pushAll();
    push_lightMatrix();
    push_minBias();
    push_maxBias();
}

void DirLight::orthoShadowMapping(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    lightProjection = glm::ortho(left, right, bottom, top, near, far);
}

void DirLight::perspectiveShadowMapping(const float fovy, const float aspect, const float near, const float far) {
    lightProjection = glm::perspective(fovy, aspect, near, far);
}

void DirLight::viewMatrixLookAt() {
    lightView = glm::lookAt(pos, lookAt, upVector);
}

void DirLight::updateLightSpaceMatrix() {
    lightSpace = lightProjection * lightView;
}

void DirLight::updateMatrices() {
    viewMatrixLookAt();
    updateLightSpaceMatrix();
}

void DirLight::configureDepthMap() {
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

void DirLight::begin() {
    bindFramebuffer(depthMapFBO);
    activeTexture(0);
	bindTexture2D(depthMap);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
}

void DirLight::end() {
    bindFramebuffer(0);
}

// struct PointLight
void PointLight::push_pos() { setVec3(cs->pointLights[index].pos, pos); }
void PointLight::push_color() { setVec3(cs->pointLights[index].color, color); }
void PointLight::push_kc() { glUniform1f(cs->pointLights[index].kc, kc); }
void PointLight::push_kl() { glUniform1f(cs->pointLights[index].kl, kl); }
void PointLight::push_kq() { glUniform1f(cs->pointLights[index].kq, kq); }
void PointLight::push_far() { glUniform1f(cs->pointLights[index].far, far); }
void PointLight::push_bias() { glUniform1f(cs->pointLights[index].bias, bias); }
void PointLight::push_shadowMap() {
    glUniform1i(cs->pointLights[index].shadowMap, textureStartId + index);
	glActiveTexture(GL_TEXTURE0 + textureStartId + index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
}

void PointLight::pushAll() {
    Light::pushAll();
    push_far();
    push_bias();
}

void PointLight::updateMatrices() {
    lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
    lightViews[0] = glm::lookAt(pos, glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec3(0, -1, 0));
    lightViews[1] = glm::lookAt(pos, glm::vec3(pos.x - 1, pos.y, pos.z), glm::vec3(0, -1, 0));
    lightViews[2] = glm::lookAt(pos, glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec3(0, 0, 1));
    lightViews[3] = glm::lookAt(pos, glm::vec3(pos.x, pos.y - 1, pos.z), glm::vec3(0, 0, -1));
    lightViews[4] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec3(0, -1, 0));
    lightViews[5] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z - 1), glm::vec3(0, -1, 0));
    for (size_t i = 0; i < 6; i++) lightSpaceMatrices[i] = lightProjection * lightViews[i];
}

void PointLight::setLightPosSS() {
    setVec3(ss->lampPos, pos);
}

void PointLight::setShadowMatricesSS() {
    for (size_t i = 0; i < 6; i++) setMat4(ss->shadowMatrices + i, lightSpaceMatrices[i]);
}

void PointLight::configureDepthMap() {
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

void PointLight::begin() {
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
}

void PointLight::end() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// struct PointLamp
void PointLamp::setPos(const glm::vec3 &pos) {
    Light::pos = pos;
    mptr->setPos(pos);
    mptr->transform();
}

// struct DirLamp
void DirLamp::setPos(const glm::vec3 &pos) {
    Light::pos = pos;
    mptr->setPos(pos);
    mptr->transform();
}

} // namespace algine
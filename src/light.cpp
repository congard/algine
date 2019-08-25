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
#include <algine/core_utils.h>
#include <algine/sconstants.h>

#define empty { /* empty */ }

namespace algine {
using namespace AlgineNames;

void Light::updateMatrices() empty

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

void Light::configureDepthMap() empty

void Light::begin() empty
	
void Light::end() empty

Light::Light() = default;

Light::~Light() {
    Framebuffer::destroy(&depthMapFBO);
    Texture::destroy(&depthMap);
    #ifdef ALGINE_LOGGING
    std::cout << "~Lamp()\n";
    #endif
}

DirLight::DirLight() {
    type = TypeDirLight;
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

PointLight::PointLight() {
    type = TypePointLight;
}

void PointLight::updateMatrices() {
    lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
    lightViews[0] = glm::lookAt(pos, glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec3(0, -1, 0));
    lightViews[1] = glm::lookAt(pos, glm::vec3(pos.x - 1, pos.y, pos.z), glm::vec3(0, -1, 0));
    lightViews[2] = glm::lookAt(pos, glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec3(0, 0, 1));
    lightViews[3] = glm::lookAt(pos, glm::vec3(pos.x, pos.y - 1, pos.z), glm::vec3(0, 0, -1));
    lightViews[4] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec3(0, -1, 0));
    lightViews[5] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z - 1), glm::vec3(0, -1, 0));
    for (size_t i = 0; i < 6; i++)
        lightSpaceMatrices[i] = lightProjection * lightViews[i];
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

LightDataSetter::LightDataSetter() = default;

LightDataSetter::LightDataSetter(const LightDataSetter &src) {
    dirLightsCount = src.dirLightsCount;
    pointLightsCount = src.pointLightsCount;
    shadowShaderPos = src.shadowShaderPos;
    shadowShaderFarPlane = src.shadowShaderFarPlane;
    shadowShaderMatrices = src.shadowShaderMatrices;

    lightLocations[0].reserve(src.lightLocations[0].size());
    for (auto i : src.lightLocations[0])
        lightLocations[0].push_back(new DirLightLocations(*reinterpret_cast<DirLightLocations*>(i)));

    lightLocations[1].reserve(src.lightLocations[1].size());
    for (auto i : src.lightLocations[1])
        lightLocations[1].push_back(new PointLightLocations(*reinterpret_cast<PointLightLocations*>(i)));
}

LightDataSetter::LightDataSetter(LightDataSetter &&src) noexcept {
    src.swap(*this);
}

LightDataSetter& LightDataSetter::operator=(const LightDataSetter &rhs) {
    if (&rhs != this)
        LightDataSetter(rhs).swap(*this);
    return *this;
}

LightDataSetter& LightDataSetter::operator=(LightDataSetter &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

LightDataSetter::~LightDataSetter() {
    clearDirLightsLocations();
    clearPointLightsLocations();
}

void LightDataSetter::swap(LightDataSetter &other) {
    std::swap(dirLightsCount, other.dirLightsCount);
    std::swap(pointLightsCount, other.pointLightsCount);
    std::swap(shadowShaderPos, other.shadowShaderPos);
    std::swap(shadowShaderFarPlane, other.shadowShaderFarPlane);
    std::swap(shadowShaderMatrices, other.shadowShaderMatrices);
    std::swap(lightLocations, other.lightLocations);
}

void LightDataSetter::clearDirLightsLocations() {
    for (auto &i : lightLocations[0])
        delete i;
    lightLocations[0].clear();
}

void LightDataSetter::clearPointLightsLocations() {
    for (auto &i : lightLocations[1])
        delete i;
    lightLocations[1].clear();
}

#define dirLightElem(elem) \
    ColorShader::DirLights + std::string("[") + std::to_string(i) + "]." + ColorShader::Light::elem

#define pointLightElem(elem) \
    ColorShader::PointLights + std::string("[") + std::to_string(i) + "]." + ColorShader::Light::elem

void LightDataSetter::indexDirLightLocations(ShaderProgram *const lightShader, const uint lightsCount) {
    clearDirLightsLocations();

    dirLightsCount = lightShader->getLocation(ColorShader::DirLightsCount);

    lightLocations[0].reserve(lightsCount);
    for (uint i = 0; i < lightsCount; ++i) {
        auto locations = new DirLightLocations;
        locations->kc = lightShader->getLocation(dirLightElem(Kc));
        locations->kl = lightShader->getLocation(dirLightElem(Kl));
        locations->kq = lightShader->getLocation(dirLightElem(Kq));
        locations->pos = lightShader->getLocation(dirLightElem(Pos));
        locations->color = lightShader->getLocation(dirLightElem(Color));
        locations->shadowMap = lightShader->getLocation(AlgineNames::ColorShader::DirLightShadowMaps + std::string("[") + std::to_string(i) + "]");
        locations->minBias = lightShader->getLocation(dirLightElem(MinBias));
        locations->maxBias = lightShader->getLocation(dirLightElem(MaxBias));
        locations->lightMatrix = lightShader->getLocation(dirLightElem(LightMatrix));
        lightLocations[0].push_back(locations);
    }
}

void LightDataSetter::indexPointLightLocations(ShaderProgram *const lightShader, ShaderProgram *const shadowShader, const uint lightsCount) {
    clearPointLightsLocations();

    pointLightsCount = lightShader->getLocation(ColorShader::PointLightsCount);

    if (shadowShader) {
        shadowShaderPos = shadowShader->getLocation(ShadowShader::PointLight::Pos);
        shadowShaderFarPlane = shadowShader->getLocation(ShadowShader::PointLight::FarPlane);
        shadowShaderMatrices = shadowShader->getLocation(ShadowShader::PointLight::ShadowMatrices);
    }

    lightLocations[1].reserve(lightsCount);
    for (uint i = 0; i < lightsCount; ++i) {
        auto locations = new PointLightLocations;
        locations->kc = lightShader->getLocation(pointLightElem(Kc));
        locations->kl = lightShader->getLocation(pointLightElem(Kl));
        locations->kq = lightShader->getLocation(pointLightElem(Kq));
        locations->pos = lightShader->getLocation(pointLightElem(Pos));
        locations->color = lightShader->getLocation(pointLightElem(Color));
        locations->shadowMap = lightShader->getLocation(AlgineNames::ColorShader::PointLightShadowMaps + std::string("[") + std::to_string(i) + "]");
        locations->farPlane = lightShader->getLocation(pointLightElem(FarPlane));
        locations->bias = lightShader->getLocation(pointLightElem(Bias));
        lightLocations[1].push_back(locations);
    }
}

void LightDataSetter::setDirLightsCount(const uint count) {
    ShaderProgram::set(dirLightsCount, count);
}

void LightDataSetter::setPointLightsCount(const uint count) {
    ShaderProgram::set(pointLightsCount, count);
}

void LightDataSetter::setKc(const Light &light, uint index) {
    ShaderProgram::set(lightLocations[light.type][index]->kc, light.kc);
}

void LightDataSetter::setKl(const Light &light, uint index) {
    ShaderProgram::set(lightLocations[light.type][index]->kl, light.kl);
}

void LightDataSetter::setKq(const Light &light, uint index) {
    ShaderProgram::set(lightLocations[light.type][index]->kq, light.kq);
}

void LightDataSetter::setPos(const Light &light, uint index) {
    ShaderProgram::set(lightLocations[light.type][index]->pos, light.pos);
}

void LightDataSetter::setColor(const Light &light, uint index) {
    ShaderProgram::set(lightLocations[light.type][index]->color, light.color);
}

void LightDataSetter::setShadowMap(const DirLight &light, uint index, uint textureSlot) {
    ShaderProgram::set(lightLocations[Light::TypeDirLight][index]->shadowMap, (int)textureSlot);
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, light.depthMap);
}

void LightDataSetter::setMinBias(const DirLight &light, uint index) {
    ShaderProgram::set(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->minBias, light.minBias);
}

void LightDataSetter::setMaxBias(const DirLight &light, uint index) {
    ShaderProgram::set(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->maxBias, light.maxBias);
}

void LightDataSetter::setLightMatrix(const DirLight &light, uint index) {
    ShaderProgram::set(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->lightMatrix, light.lightSpace);
}

void LightDataSetter::setShadowMap(const PointLight &light, uint index, uint textureSlot) {
    ShaderProgram::set(lightLocations[Light::TypePointLight][index]->shadowMap, (int)textureSlot);
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.depthMap);
}

void LightDataSetter::setFarPlane(const PointLight &light, uint index) {
    ShaderProgram::set(reinterpret_cast<PointLightLocations*>(lightLocations[Light::TypePointLight][index])->farPlane, light.far);
}

void LightDataSetter::setBias(const PointLight &light, uint index) {
    ShaderProgram::set(reinterpret_cast<PointLightLocations*>(lightLocations[Light::TypePointLight][index])->bias, light.bias);
}

void LightDataSetter::setShadowShaderPos(const PointLight &light) {
    ShaderProgram::set(shadowShaderPos, light.pos);
}

void LightDataSetter::setShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::set(shadowShaderFarPlane, light.far);
}

void LightDataSetter::setShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++)
        ShaderProgram::set(shadowShaderMatrices + i, light.lightSpaceMatrices[i]);
}

#define checkIsDirLight \
    if (lightType != Light::TypeDirLight) { \
        std::cerr << "Object " << obj << " can only be used with Light::TypeDirLight\n"; \
        return -1; \
    }

#define checkIsPointLight \
    if (lightType != Light::TypePointLight) { \
        std::cerr << "Object " << obj << " can only be used with Light::TypePointLight\n"; \
        return -1; \
    }

int LightDataSetter::getLocation(const uint obj, const uint lightType, const uint lightIndex) {
    switch (obj) {
        case DirLightsCount:
            return dirLightsCount;
        case PointLightsCount:
            return pointLightsCount;
        case Kc:
            return lightLocations[lightType][lightIndex]->kc;
        case Kl:
            return lightLocations[lightType][lightIndex]->kl;
        case Kq:
            return lightLocations[lightType][lightIndex]->kq;
        case Pos:
            return lightLocations[lightType][lightIndex]->pos;
        case Color:
            return lightLocations[lightType][lightIndex]->color;
        case ShadowMap:
            return lightLocations[lightType][lightIndex]->shadowMap;
        case MinBias:
            checkIsDirLight
            return reinterpret_cast<DirLightLocations*>(lightLocations[lightType][lightIndex])->minBias;
        case MaxBias:
            checkIsDirLight
            return reinterpret_cast<DirLightLocations*>(lightLocations[lightType][lightIndex])->maxBias;
        case LightMatrix:
            checkIsDirLight
            return reinterpret_cast<DirLightLocations*>(lightLocations[lightType][lightIndex])->lightMatrix;
        case Bias:
            checkIsPointLight
            return reinterpret_cast<PointLightLocations*>(lightLocations[lightType][lightIndex])->bias;
        case FarPlane:
            checkIsPointLight
            return reinterpret_cast<PointLightLocations*>(lightLocations[lightType][lightIndex])->farPlane;
        case ShadowShaderPos:
            checkIsPointLight
            return shadowShaderPos;
        case ShadowShaderFarPlane:
            checkIsPointLight
            return shadowShaderFarPlane;
        case ShadowShaderMatrices:
            checkIsPointLight
            return shadowShaderMatrices;
        default:
            std::cerr << "Object " << obj << " not found\n";
            return -1;
    }
}

void PointLamp::setPos(const glm::vec3 &pos) {
    Light::pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}

void DirLamp::setPos(const glm::vec3 &pos) {
    Light::pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}

} // namespace algine

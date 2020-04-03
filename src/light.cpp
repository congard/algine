#define GLM_FORCE_CTOR_INIT
#include <algine/light.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <algine/framebuffer.h>
#include <algine/texture.h>
#include <algine/constants/Lighting.h>
#include <algine/constants/ShadowShader.h>

using namespace std;
namespace Lighting = algine::Module::Lighting::Vars;

namespace algine {
Light::~Light() {
    Framebuffer::destroy(shadowMapFb);
}

void Light::translate() { // TODO: check rotate
    m_translation = glm::translate(glm::mat4(1.0f), -m_pos);
}

void Light::end() {
    shadowMapFb->unbind();
}

void Light::setKc(const float kc) {
    m_kc = kc;
}

void Light::setKl(const float kl) {
    m_kl = kl;
}

void Light::setKq(const float kq) {
    m_kq = kq;
}

void Light::setColor(const glm::vec3 &color) {
    m_color = color;
}

void Light::setColor(const float red, const float green, const float blue) {
    m_color.r = red;
    m_color.g = green;
    m_color.b = blue;
}

float Light::getKc() {
    return m_kc;
}

float Light::getKl() {
    return m_kl;
}

float Light::getKq() {
    return m_kq;
}

glm::vec3 Light::getColor() {
    return m_color;
}

DirLight::DirLight(const uint rotatorType): rotatable(rotatorType) {
    type = TypeDirLight;
}

DirLight::~DirLight() {
    Texture2D::destroy(shadowMap);
}

void DirLight::orthoShadows(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    m_lightProjection = glm::ortho(left, right, bottom, top, near, far);
}

void DirLight::perspectiveShadows(const float fovy, const float aspect, const float near, const float far) {
    m_lightProjection = glm::perspective(fovy, aspect, near, far);
}

void DirLight::initShadows(const uint shadowMapWidth, const uint shadowMapHeight) {
    shadowMap = new Texture2D();
    shadowMapFb = new Framebuffer();

    shadowMap->bind();
    shadowMap->setWidthHeight(shadowMapWidth, shadowMapHeight);
    shadowMap->setFormat(Texture::DepthComponent);
    shadowMap->update();
    shadowMap->setParams(map<uint, uint> {
            pair<uint, uint> {Texture::MinFilter, Texture::Nearest},
            pair<uint, uint> {Texture::MagFilter, Texture::Nearest},
            pair<uint, uint> {Texture::WrapU, Texture::Repeat},
            pair<uint, uint> {Texture::WrapV, Texture::Repeat}
    });
    shadowMap->unbind();

    shadowMapFb->bind();
    shadowMapFb->attachTexture(shadowMap, Framebuffer::DepthAttachment);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    shadowMapFb->unbind();
}

void DirLight::updateMatrix() {
    m_lightSpace = m_lightProjection * m_rotation * m_translation;
}

void DirLight::begin() {
    shadowMapFb->bind();
	glViewport(0, 0, shadowMap->width, shadowMap->height);
}

void DirLight::setMinBias(const float minBias) {
    m_minBias = minBias;
}

void DirLight::setMaxBias(const float maxBias) {
    m_maxBias = maxBias;
}

float DirLight::getMinBias() const {
    return m_minBias;
}

float DirLight::getMaxBias() const {
    return m_maxBias;
}

#define lightView(l_x, l_y, l_z, up_x, up_y, up_z) \
glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(l_x, l_y, l_z), glm::vec3(up_x, up_y, up_z))

const glm::mat4 PointLight::m_lightViews[] = {
        lightView( 1,  0,  0,   0, -1,  0 ),
        lightView(-1,  0,  0,   0, -1,  0 ),
        lightView( 0,  1,  0,   0,  0,  1 ),
        lightView( 0, -1,  0,   0,  0, -1 ),
        lightView( 0,  0,  1,   0, -1,  0 ),
        lightView( 0,  0, -1,   0, -1,  0 )
};

#undef lightView

PointLight::PointLight() {
    type = TypePointLight;
}

PointLight::~PointLight() {
    TextureCube::destroy(shadowMap);
}

void PointLight::initShadows(const uint shadowMapWidth, const uint shadowMapHeight) {
    shadowMap = new TextureCube();
    shadowMapFb = new Framebuffer();

    shadowMap->bind();
    shadowMap->setWidthHeight(shadowMapWidth, shadowMapHeight);
    shadowMap->setFormat(Texture::DepthComponent);
    shadowMap->update();
    shadowMap->setParams(map<uint, uint> {
            pair<uint, uint> {Texture::MinFilter, Texture::Nearest},
            pair<uint, uint> {Texture::MagFilter, Texture::Nearest},
            pair<uint, uint> {Texture::WrapU, Texture::ClampToEdge},
            pair<uint, uint> {Texture::WrapV, Texture::ClampToEdge},
            pair<uint, uint> {Texture::WrapW, Texture::ClampToEdge}
    });
    shadowMap->unbind();

    shadowMapFb->bind();
    shadowMapFb->attachTexture(shadowMap, Framebuffer::DepthAttachment);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    shadowMapFb->unbind();
}

void PointLight::updateMatrix() {
    for (size_t i = 0; i < 6; i++)
        m_lightSpaceMatrices[i] = m_lightProjection * m_lightViews[i] * m_translation;
}

void PointLight::begin() {
	shadowMapFb->bind();
    glViewport(0, 0, shadowMap->width, shadowMap->height);
}

void PointLight::perspectiveShadows() {
    m_lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, m_near, m_far);
}

void PointLight::setFar(const float far) {
    m_far = far;
}

void PointLight::setNear(const float near) {
    m_near = near;
}

void PointLight::setBias(const float bias) {
    m_bias = bias;
}

float PointLight::getFar() const {
    return m_far;
}

float PointLight::getNear() const {
    return m_near;
}

float PointLight::getBias() const {
    return m_bias;
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
    Lighting::DirLights + std::string("[") + std::to_string(i) + "]." + Lighting::Light::elem

#define pointLightElem(elem) \
    Lighting::PointLights + std::string("[") + std::to_string(i) + "]." + Lighting::Light::elem

void LightDataSetter::indexDirLightLocations(ShaderProgram *const lightShader, const uint lightsCount) {
    clearDirLightsLocations();

    dirLightsCount = lightShader->getLocation(Lighting::DirLightsCount);

    lightLocations[0].reserve(lightsCount);
    for (uint i = 0; i < lightsCount; ++i) {
        auto locations = new DirLightLocations;
        locations->kc = lightShader->getLocation(dirLightElem(Kc));
        locations->kl = lightShader->getLocation(dirLightElem(Kl));
        locations->kq = lightShader->getLocation(dirLightElem(Kq));
        locations->pos = lightShader->getLocation(dirLightElem(Pos));
        locations->color = lightShader->getLocation(dirLightElem(Color));
        locations->shadowMap = lightShader->getLocation(Lighting::DirLightShadowMaps) + i;
        locations->minBias = lightShader->getLocation(dirLightElem(MinBias));
        locations->maxBias = lightShader->getLocation(dirLightElem(MaxBias));
        locations->lightMatrix = lightShader->getLocation(dirLightElem(LightMatrix));
        lightLocations[0].push_back(locations);
    }
}

void LightDataSetter::indexPointLightLocations(ShaderProgram *const lightShader, ShaderProgram *const shadowShader, const uint lightsCount) {
    clearPointLightsLocations();

    pointLightsCount = lightShader->getLocation(Lighting::PointLightsCount);

    if (shadowShader) {
        shadowShaderPos = shadowShader->getLocation(ShadowShader::Vars::PointLight::Pos);
        shadowShaderFarPlane = shadowShader->getLocation(ShadowShader::Vars::PointLight::FarPlane);
        shadowShaderMatrices = shadowShader->getLocation(ShadowShader::Vars::PointLight::ShadowMatrices);
    }

    lightLocations[1].reserve(lightsCount);
    for (uint i = 0; i < lightsCount; ++i) {
        auto locations = new PointLightLocations;
        locations->kc = lightShader->getLocation(pointLightElem(Kc));
        locations->kl = lightShader->getLocation(pointLightElem(Kl));
        locations->kq = lightShader->getLocation(pointLightElem(Kq));
        locations->pos = lightShader->getLocation(pointLightElem(Pos));
        locations->color = lightShader->getLocation(pointLightElem(Color));
        locations->shadowMap = lightShader->getLocation(Lighting::PointLightShadowMaps) + i;
        locations->farPlane = lightShader->getLocation(pointLightElem(FarPlane));
        locations->bias = lightShader->getLocation(pointLightElem(Bias));
        lightLocations[1].push_back(locations);
    }
}

void LightDataSetter::setDirLightsCount(const uint count) {
    ShaderProgram::setUint(dirLightsCount, count);
}

void LightDataSetter::setPointLightsCount(const uint count) {
    ShaderProgram::setUint(pointLightsCount, count);
}

void LightDataSetter::setKc(const Light &light, uint index) {
    ShaderProgram::setFloat(lightLocations[light.type][index]->kc, light.m_kc);
}

void LightDataSetter::setKl(const Light &light, uint index) {
    ShaderProgram::setFloat(lightLocations[light.type][index]->kl, light.m_kl);
}

void LightDataSetter::setKq(const Light &light, uint index) {
    ShaderProgram::setFloat(lightLocations[light.type][index]->kq, light.m_kq);
}

void LightDataSetter::setPos(const Light &light, uint index) {
    ShaderProgram::setVec3(lightLocations[light.type][index]->pos, light.m_pos);
}

void LightDataSetter::setColor(const Light &light, uint index) {
    ShaderProgram::setVec3(lightLocations[light.type][index]->color, light.m_color);
}

void LightDataSetter::setShadowMap(const DirLight &light, uint index, uint textureSlot) {
    ShaderProgram::setInt(lightLocations[Light::TypeDirLight][index]->shadowMap, textureSlot);
    light.shadowMap->use(textureSlot);
}

void LightDataSetter::setMinBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->minBias, light.m_minBias);
}

void LightDataSetter::setMaxBias(const DirLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->maxBias, light.m_maxBias);
}

void LightDataSetter::setLightMatrix(const DirLight &light, uint index) {
    ShaderProgram::setMat4(reinterpret_cast<DirLightLocations*>(lightLocations[Light::TypeDirLight][index])->lightMatrix, light.m_lightSpace);
}

void LightDataSetter::setShadowMap(const PointLight &light, uint index, uint textureSlot) {
    ShaderProgram::setInt(lightLocations[Light::TypePointLight][index]->shadowMap, textureSlot);
    light.shadowMap->use(textureSlot);
}

void LightDataSetter::setFarPlane(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<PointLightLocations*>(lightLocations[Light::TypePointLight][index])->farPlane, light.m_far);
}

void LightDataSetter::setBias(const PointLight &light, uint index) {
    ShaderProgram::setFloat(reinterpret_cast<PointLightLocations*>(lightLocations[Light::TypePointLight][index])->bias, light.m_bias);
}

void LightDataSetter::setShadowShaderPos(const PointLight &light) {
    ShaderProgram::setVec3(shadowShaderPos, light.m_pos);
}

void LightDataSetter::setShadowShaderFarPlane(const PointLight &light) {
    ShaderProgram::setFloat(shadowShaderFarPlane, light.m_far);
}

void LightDataSetter::setShadowShaderMatrices(const PointLight &light) {
    for (int i = 0; i < 6; i++)
        ShaderProgram::setMat4(shadowShaderMatrices + i, light.m_lightSpaceMatrices[i]);
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
    Light::m_pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}

void DirLamp::setPos(const glm::vec3 &pos) {
    Light::m_pos = pos;
    mptr->setPos(pos);
    mptr->translate();
}

} // namespace algine

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

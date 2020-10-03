#define GLM_FORCE_CTOR_INIT

#include <algine/std/lighting/DirLight.h>

#include <algine/core/PtrMaker.h>

#include <glm/gtc/matrix_transform.hpp>
#include <map>

using namespace std;

namespace algine {
DirLight::DirLight(uint rotatorType)
    : Rotatable(rotatorType)
{
    m_type = Type::Dir;
}

void DirLight::orthoShadows(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    m_lightProjection = glm::ortho(left, right, bottom, top, near, far);
}

void DirLight::perspectiveShadows(const float fovy, const float aspect, const float near, const float far) {
    m_lightProjection = glm::perspective(fovy, aspect, near, far);
}

void DirLight::initShadows(const uint shadowMapWidth, const uint shadowMapHeight) {
    Texture2DPtr shadowMap = PtrMaker::make();
    m_shadowFb = new Framebuffer();

    shadowMap->bind();
    shadowMap->setDimensions(shadowMapWidth, shadowMapHeight);
    shadowMap->setFormat(Texture::DepthComponent);
    shadowMap->update();
    shadowMap->setParams(map<uint, uint> {
        {Texture::MinFilter, Texture::Nearest},
        {Texture::MagFilter, Texture::Nearest},
        {Texture::WrapU, Texture::Repeat},
        {Texture::WrapV, Texture::Repeat}
    });
    shadowMap->unbind();

    m_shadowFb->bind();
    m_shadowFb->attachTexture(shadowMap, Framebuffer::DepthAttachment);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    m_shadowFb->unbind();
}

void DirLight::updateMatrix() {
    m_lightSpace = m_lightProjection * m_rotation * m_translation;
}

void DirLight::begin() {
    m_shadowFb->bind();

    const auto &shadowMap = getShadowMap();

    glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());
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

Texture2DPtr& DirLight::getShadowMap() const {
    return m_shadowFb->getAttachedTexture2D(Framebuffer::DepthAttachment);
}

const glm::mat4& DirLight::getLightSpaceMatrix() const {
    return m_lightSpace;
}
}

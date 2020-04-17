#define GLM_FORCE_CTOR_INIT

#include <algine/lighting/DirLight.h>
#include <map>

using namespace std;

namespace algine {
namespace Lighting {
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
}
}

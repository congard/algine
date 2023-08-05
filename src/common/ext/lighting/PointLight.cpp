#include <algine/ext/lighting/PointLight.h>

#include <algine/core/PtrMaker.h>

#include <glm/gtc/matrix_transform.hpp>
#include <map>

using namespace std;

namespace algine {
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

PointLight::PointLight(Object *parent)
    : Light(parent),
      m_lightSpaceMatrices()
{
    m_type = Type::Point;
}

void PointLight::initShadows(const uint shadowMapWidth, const uint shadowMapHeight) {
    m_shadowFb = new Framebuffer(this);
    auto shadowMap = new TextureCube(m_shadowFb);

    shadowMap->bind();
    shadowMap->setDimensions(shadowMapWidth, shadowMapHeight);
    shadowMap->setFormat(Texture::DepthComponent);
    shadowMap->update();
    shadowMap->setParams(map<uint, uint> {
        {Texture::MinFilter, Texture::Nearest},
        {Texture::MagFilter, Texture::Nearest},
        {Texture::WrapU, Texture::ClampToEdge},
        {Texture::WrapV, Texture::ClampToEdge},
        {Texture::WrapW, Texture::ClampToEdge}
    });
    shadowMap->unbind();

    m_shadowFb->bind();
    m_shadowFb->attachTexture(shadowMap, Framebuffer::DepthAttachment);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    m_shadowFb->unbind();
}

void PointLight::updateMatrix() {
    for (size_t i = 0; i < 6; i++) {
        m_lightSpaceMatrices[i] = m_lightProjection * m_lightViews[i] * m_translation;
    }
}

void PointLight::begin() {
    m_shadowFb->bind();

    const auto &shadowMap = getShadowMap();

    glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());
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

TextureCube* PointLight::getShadowMap() const {
    return m_shadowFb->getAttachedTextureCube(Framebuffer::DepthAttachment);
}

const glm::mat4& PointLight::getLightSpaceMatrix(TextureCube::Face face) const {
    return m_lightSpaceMatrices[static_cast<uint>(face)];
}
}

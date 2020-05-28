#define GLM_FORCE_CTOR_INIT

#include <algine/std/lighting/PointLight.h>

#include <glm/gtc/matrix_transform.hpp>
#include <map>

using namespace std;

namespace algine {
namespace Lighting {
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
}
}

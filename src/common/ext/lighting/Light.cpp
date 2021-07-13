#include <algine/ext/lighting/Light.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
Light::~Light() {
    delete m_shadowFb;
}

void Light::translate() { // TODO: check rotate
    m_translation = glm::translate(glm::mat4(1.0f), -m_pos);
}

void Light::end() {
    m_shadowFb->unbind();
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

float Light::getKc() const {
    return m_kc;
}

float Light::getKl() const {
    return m_kl;
}

float Light::getKq() const {
    return m_kq;
}

glm::vec3 Light::getColor() const {
    return m_color;
}

Framebuffer* Light::getShadowFramebuffer() const {
    return m_shadowFb;
}

const glm::mat4& Light::getLightProjection() const {
    return m_lightProjection;
}
}

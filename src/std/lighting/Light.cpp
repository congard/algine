#define GLM_FORCE_CTOR_INIT

#include <algine/std/lighting/Light.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
namespace Lighting {
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
}
}

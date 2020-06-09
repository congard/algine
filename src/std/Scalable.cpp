#define GLM_FORCE_CTOR_INIT
#include <algine/std/Scalable.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
void Scalable::setScale(const float x, const float y, const float z) {
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

void Scalable::setScale(const glm::vec3 &scale) {
    m_scale = scale;
}

void Scalable::setScaleX(const float x) {
    m_scale.x = x;
}

void Scalable::setScaleY(const float y) {
    m_scale.y = y;
}

void Scalable::setScaleZ(const float z) {
    m_scale.z = z;
}

void Scalable::scale() {
    m_scaling = glm::scale(glm::mat4(), m_scale);
}

float Scalable::getScaleX() const {
    return m_scale.x;
}

float Scalable::getScaleY() const {
    return m_scale.y;
}

float Scalable::getScaleZ() const {
    return m_scale.z;
}

glm::vec3 Scalable::getScale() const {
    return m_scale;
}

glm::mat4 Scalable::getScalingMatrix() const {
    return m_scaling;
}
}

#define GLM_FORCE_CTOR_INIT
#include <algine/std/Translatable.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
void Translatable::setPos(const float x, const float y, const float z) {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void Translatable::setPos(const glm::vec3 &pos) {
    m_pos = pos;
}

void Translatable::setX(const float x) {
    m_pos.x = x;
}

void Translatable::setY(const float y) {
    m_pos.y = y;
}

void Translatable::setZ(const float z) {
    m_pos.z = z;
}

void Translatable::translate() {
    m_translation = glm::translate(glm::mat4(), m_pos);
}

float Translatable::getX() const {
    return m_pos.x;
}

float Translatable::getY() const {
    return m_pos.y;
}

float Translatable::getZ() const {
    return m_pos.z;
}

glm::vec3 Translatable::getPos() const {
    return m_pos;
}

glm::mat4 Translatable::getTranslationMatrix() const {
    return m_translation;
}
}

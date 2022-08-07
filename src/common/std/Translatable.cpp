#include <algine/std/Translatable.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

namespace algine {
Translatable::Translatable()
    : m_pos(0.0f),
      m_translation(1.0f) {}

void Translatable::setPos(float x, float y, float z) {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void Translatable::setPos(const vec3 &pos) {
    m_pos = pos;
}

void Translatable::setX(float x) {
    m_pos.x = x;
}

void Translatable::setY(float y) {
    m_pos.y = y;
}

void Translatable::setZ(float z) {
    m_pos.z = z;
}

void Translatable::changePos(const vec3 &dPos) {
    m_pos += dPos;
}

void Translatable::translate() {
    m_translation = glm::translate(mat4(1.0f), m_pos);
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

const vec3& Translatable::getPos() const {
    return m_pos;
}

const mat4& Translatable::getTranslationMatrix() const {
    return m_translation;
}
}

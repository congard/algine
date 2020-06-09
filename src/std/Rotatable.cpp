#define GLM_FORCE_CTOR_INIT
#include <algine/std/Rotatable.h>

#include <tulz/macros.h>
#include <algorithm>

namespace algine {
void Rotatable::swap(algine::Rotatable &other) {
    std::swap(m_rotation, other.m_rotation);
    std::swap(rotator, other.rotator);
}

Rotatable::Rotatable(const uint rotatorType) {
    rotator = Rotator::create(rotatorType);
}

Rotatable::Rotatable(const Rotatable &src) {
    m_rotation = src.m_rotation;
    rotator = Rotator::create(src.rotator->type);
}

Rotatable::Rotatable(Rotatable &&src) noexcept {
    src.swap(*this);
}

Rotatable& Rotatable::operator=(const Rotatable &rhs) {
    if (&rhs != this)
        Rotatable(rhs).swap(*this);
    return *this;
}

Rotatable& Rotatable::operator=(Rotatable &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

Rotatable::~Rotatable() {
    deletePtr(rotator)
}

void Rotatable::setPitch(const float pitch) {
    rotator->pitch = pitch;
}

void Rotatable::setYaw(const float yaw) {
    rotator->yaw = yaw;
}

void Rotatable::setRoll(const float roll) {
    rotator->roll = roll;
}

void Rotatable::setRotate(const float pitch, const float yaw, const float roll) {
    rotator->pitch = pitch;
    rotator->yaw = yaw;
    rotator->roll = roll;
}

void Rotatable::rotate() {
    rotator->rotate(m_rotation);
}

float Rotatable::getPitch() const {
    return rotator->pitch;
}

float Rotatable::getYaw() const {
    return rotator->yaw;
}

float Rotatable::getRoll() const {
    return rotator->roll;
}

glm::mat4 Rotatable::getRotationMatrix() const {
    return m_rotation;
}

glm::vec3 Rotatable::getRotate() const {
    return glm::vec3(rotator->pitch, rotator->yaw, rotator->roll);
}

glm::vec3 Rotatable::getForward() const {
    return -glm::vec3(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
}

glm::vec3 Rotatable::getBack() const {
    return -getForward();
}

glm::vec3 Rotatable::getRight() const {
    return glm::vec3(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
}

glm::vec3 Rotatable::getLeft() const {
    return -getRight();
}
}

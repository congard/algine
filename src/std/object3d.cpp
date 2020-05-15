#define GLM_FORCE_CTOR_INIT

#include <algine/std/object3d.h>
#include <tulz/macros.h>
#include <algorithm>

namespace algine {
void rotatable::swap(algine::rotatable &other) {
    std::swap(m_rotation, other.m_rotation);
    std::swap(rotator, other.rotator);
}

rotatable::rotatable(const uint rotatorType) {
    rotator = Rotator::create(rotatorType);
}

rotatable::rotatable(const rotatable &src) {
    m_rotation = src.m_rotation;
    rotator = Rotator::create(src.rotator->type);
}

rotatable::rotatable(rotatable &&src) noexcept {
    src.swap(*this);
}

rotatable& rotatable::operator=(const rotatable &rhs) {
    if (&rhs != this)
        rotatable(rhs).swap(*this);
    return *this;
}

rotatable& rotatable::operator=(rotatable &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

rotatable::~rotatable() {
    deletePtr(rotator)
}

void rotatable::setPitch(const float pitch) {
    rotator->pitch = pitch;
}

void rotatable::setYaw(const float yaw) {
    rotator->yaw = yaw;
}

void rotatable::setRoll(const float roll) {
    rotator->roll = roll;
}

void rotatable::setRotate(const float pitch, const float yaw, const float roll) {
    rotator->pitch = pitch;
    rotator->yaw = yaw;
    rotator->roll = roll;
}

void rotatable::rotate() {
    rotator->rotate(m_rotation);
}

glm::vec3 rotatable::getRotate() const {
    return glm::vec3(rotator->pitch, rotator->yaw, rotator->roll);
}

float rotatable::getPitch() const {
    return rotator->pitch;
}

float rotatable::getYaw() const {
    return rotator->yaw;
}

float rotatable::getRoll() const {
    return rotator->roll;
}

glm::mat4 rotatable::getRotationMatrix() const {
    return m_rotation;
}

glm::vec3 rotatable::getForward() const {
    return -glm::vec3(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
}

glm::vec3 rotatable::getBack() const {
    return -getForward();
}

glm::vec3 rotatable::getRight() const {
    return glm::vec3(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
}

glm::vec3 rotatable::getLeft() const {
    return -getRight();
}

void translatable::setPos(const float x, const float y, const float z) {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void translatable::setPos(const glm::vec3 &pos) {
    m_pos = pos;
}

void translatable::setX(const float x) {
    m_pos.x = x;
}

void translatable::setY(const float y) {
    m_pos.y = y;
}

void translatable::setZ(const float z) {
    m_pos.z = z;
}

void translatable::translate() {
    m_translation = glm::translate(glm::mat4(), m_pos);
}

glm::vec3 translatable::getPos() const {
    return m_pos;
}

float translatable::getX() const {
    return m_pos.x;
}

float translatable::getY() const {
    return m_pos.y;
}

float translatable::getZ() const {
    return m_pos.z;
}

glm::mat4 translatable::getTranslationMatrix() const {
    return m_translation;
}

void scalable::setScale(const float x, const float y, const float z) {
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

void scalable::setScale(const glm::vec3 &scale) {
    m_scale = scale;
}

void scalable::setScaleX(const float x) {
    m_scale.x = x;
}

void scalable::setScaleY(const float y) {
    m_scale.y = y;
}

void scalable::setScaleZ(const float z) {
    m_scale.z = z;
}

void scalable::scale() {
    m_scaling = glm::scale(glm::mat4(), m_scale);
}

glm::vec3 scalable::getScale() const {
    return m_scale;
}

float scalable::getScaleX() const {
    return m_scale.x;
}

float scalable::getScaleY() const {
    return m_scale.y;
}

float scalable::getScaleZ() const {
    return m_scale.z;
}

glm::mat4 scalable::getScalingMatrix() const {
    return m_scaling;
}
}

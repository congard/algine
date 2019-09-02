#define GLM_FORCE_CTOR_INIT

#include <algine/object3d.h>
#include <algine/core_utils.h>

namespace algine {
Object3D::Object3D(const uint rotatorType) {
    rotator = Rotator::create(rotatorType);
}

Object3D::Object3D(const Object3D &src) {
    m_rotation = src.m_rotation;
    m_translation = src.m_translation;
    m_scaling = src.m_scaling;
    m_transform = src.m_transform;
    v_pos = src.v_pos;
    v_scale = src.v_scale;
    type = src.type;
    rotator = Rotator::create(src.rotator->type);
}

Object3D::Object3D(Object3D &&src) noexcept {
    src.swap(*this);
}

Object3D& Object3D::operator=(const Object3D &rhs) {
    if (&rhs != this)
        Object3D(rhs).swap(*this);
    return *this;
}

Object3D& Object3D::operator=(Object3D &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

Object3D::~Object3D() {
    delete_ptr(rotator)
}

void Object3D::swap(Object3D &other) {
    std::swap(m_rotation, other.m_rotation);
    std::swap(m_translation, other.m_translation);
    std::swap(m_scaling, other.m_scaling);
    std::swap(m_transform, other.m_transform);
    std::swap(v_pos, other.v_pos);
    std::swap(v_scale, other.v_scale);
    std::swap(type, other.type);
    std::swap(rotator, other.rotator);
}

void Object3D::setPitch(const float pitch) {
    rotator->pitch = pitch;
}

void Object3D::setYaw(const float yaw) {
    rotator->yaw = yaw;
}

void Object3D::setRoll(const float roll) {
    rotator->roll = roll;
}

void Object3D::setRotate(const float pitch, const float yaw, const float roll) {
    rotator->pitch = pitch;
    rotator->yaw = yaw;
    rotator->roll = roll;
}

void Object3D::rotate() {
    rotator->rotate(m_rotation);
}

void Object3D::setPos(const float x, const float y, const float z) {
    v_pos.x = x;
    v_pos.y = y;
    v_pos.z = z;
}

void Object3D::setPos(const glm::vec3 &pos) {
    v_pos = pos;
}

void Object3D::setX(const float x) {
    v_pos.x = x;
}

void Object3D::setY(const float y) {
    v_pos.y = y;
}

void Object3D::setZ(const float z) {
    v_pos.z = z;
}

void Object3D::translate() {
    m_translation = glm::translate(glm::mat4(), v_pos);
}

void Object3D::setScale(const float x, const float y, const float z) {
    v_scale.x = x;
    v_scale.y = y;
    v_scale.z = z;
}

void Object3D::setScale(const glm::vec3 &scale) {
    v_scale = scale;
}

void Object3D::setScaleX(const float x) {
    v_scale.x = x;
}

void Object3D::setScaleY(const float y) {
    v_scale.y = y;
}

void Object3D::setScaleZ(const float z) {
    v_scale.z = z;
}

void Object3D::scale() {
    m_scaling = glm::scale(glm::mat4(), v_scale);
}

void Object3D::updateMatrix() {
    m_transform = m_translation * m_rotation * m_scaling;
}

glm::vec3 Object3D::getPosition() {
    return v_pos;
}

float Object3D::getX() {
    return v_pos.x;
}

float Object3D::getY() {
    return v_pos.y;
}

float Object3D::getZ() {
    return v_pos.z;
}

glm::vec3 Object3D::getScale() {
    return v_scale;
}

float Object3D::getScaleX() {
    return v_scale.x;
}

float Object3D::getScaleY() {
    return v_scale.y;
}

float Object3D::getScaleZ() {
    return v_scale.z;
}

glm::vec3 Object3D::getRotate() {
    return glm::vec3(rotator->pitch, rotator->yaw, rotator->roll);
}

float Object3D::getPitch() {
    return rotator->pitch;
}

float Object3D::getYaw() {
    return rotator->yaw;
}

float Object3D::getRoll() {
    return rotator->roll;
}

glm::mat4 Object3D::getRotationMatrix() {
    return m_rotation;
}

glm::mat4 Object3D::getTranslationMatrix() {
    return m_translation;
}

glm::mat4 Object3D::getScalingMatrix() {
    return m_scaling;
}

glm::mat4 Object3D::getMatrix() {
    return m_transform;
}

glm::vec3 Object3D::getForward() {
    return -glm::vec3(m_rotation[0][2], m_rotation[1][2], m_rotation[2][2]);
}

glm::vec3 Object3D::getBack() {
    return -getForward();
}

glm::vec3 Object3D::getRight() {
    return glm::vec3(m_rotation[0][0], m_rotation[1][0], m_rotation[2][0]);
}

glm::vec3 Object3D::getLeft() {
    return -getRight();
}
}

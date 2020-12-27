#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
void Camera::translate() {
    m_translation = glm::translate(glm::mat4(1.0f), -m_pos);
}

void Camera::updateMatrix() {
    m_transform = m_rotation * m_translation * m_scaling;
}

void Camera::goForward() {
    m_pos += getForward() * m_speedForward;
}

void Camera::goBack() {
    m_pos += getBack() * m_speedBack;
}

void Camera::goRight() {
    m_pos += getRight() * m_speedRight;
}

void Camera::goLeft() {
    m_pos += getLeft() * m_speedLeft;
}

void Camera::setFieldOfView(float fov) {
    m_fov = fov;
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

void Camera::setNear(float near) {
    m_near = near;
}

void Camera::setFar(float far) {
    m_far = far;
}

void Camera::perspective() {
    m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
}

void Camera::setSensitivityPitch(float sensitivityPitch) {
    m_sensitivityPitch = sensitivityPitch;
}

void Camera::setSensitivityYaw(float sensitivityYaw) {
    m_sensitivityYaw = sensitivityYaw;
}

void Camera::setSensitivityRoll(float sensitivityRoll) {
    m_sensitivityRoll = sensitivityRoll;
}

void Camera::setSpeedForward(float speedForward) {
    m_speedForward = speedForward;
}

void Camera::setSpeedBack(float speedBack) {
    m_speedBack = speedBack;
}

void Camera::setSpeedRight(float speedRight) {
    m_speedRight = speedRight;
}

void Camera::setSpeedLeft(float speedLeft) {
    m_speedLeft = speedLeft;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return m_projection;
}

glm::mat4 Camera::getViewMatrix() const {
    return m_transform;
}

float Camera::getFieldOfView() const {
    return m_fov;
}

float Camera::getAspectRatio() const {
    return m_aspectRatio;
}

float Camera::getNear() const {
    return m_near;
}

float Camera::getFar() const {
    return m_far;
}

float Camera::getSensitivityPitch() const {
    return m_sensitivityPitch;
}

float Camera::getSensitivityYaw() const {
    return m_sensitivityYaw;
}

float Camera::getSensitivityRoll() const {
    return m_sensitivityRoll;
}

float Camera::getSpeedForward() const {
    return m_speedForward;
}

float Camera::getSpeedBack() const {
    return m_speedBack;
}

float Camera::getSpeedRight() const {
    return m_speedRight;
}

float Camera::getSpeedLeft() const {
    return m_speedLeft;
}

Camera::Camera(Rotator::Type rotatorType)
    : Rotatable(rotatorType),
      m_projection(1.0f),
      m_transform(1.0f),
      m_fov(glm::radians(90.0f)),
      m_aspectRatio(1.0f),
      m_near(1.0f),
      m_far(32.0f),
      m_sensitivityPitch(0.0025f),
      m_sensitivityYaw(0.0025f),
      m_sensitivityRoll(0.0025f),
      m_speedForward(1.0f),
      m_speedBack(1.0f),
      m_speedRight(1.0f),
      m_speedLeft(1.0f) {}
}

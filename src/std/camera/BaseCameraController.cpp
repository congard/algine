#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/BaseCameraController.h>

namespace algine {
BaseCameraController::BaseCameraController()
    : m_camera(nullptr),
      m_sensitivityPitch(0.0025f),
      m_sensitivityYaw(0.0025f),
      m_sensitivityRoll(0.0025f),
      m_speedForward(1.0f),
      m_speedBack(1.0f),
      m_speedRight(1.0f),
      m_speedLeft(1.0f),
      m_lastMousePos(0.0f) {}

void BaseCameraController::mouseMove(float x, float y) {
    mouseMove(x, y, 0);
}

void BaseCameraController::setMousePos(float x, float y, float z) {
    m_lastMousePos = glm::vec3(x, y, z);
}

void BaseCameraController::goForward() {
    m_camera->m_pos += m_camera->getForward() * m_speedForward;
}

void BaseCameraController::goBack() {
    m_camera->m_pos += m_camera->getBack() * m_speedBack;
}

void BaseCameraController::goRight() {
    m_camera->m_pos += m_camera->getRight() * m_speedRight;
}

void BaseCameraController::goLeft() {
    m_camera->m_pos += m_camera->getLeft() * m_speedLeft;
}

void BaseCameraController::setCamera(Camera *camera) {
    m_camera = camera;
}

void BaseCameraController::setSensitivityPitch(float sensitivityPitch) {
    m_sensitivityPitch = sensitivityPitch;
}

void BaseCameraController::setSensitivityYaw(float sensitivityYaw) {
    m_sensitivityYaw = sensitivityYaw;
}

void BaseCameraController::setSensitivityRoll(float sensitivityRoll) {
    m_sensitivityRoll = sensitivityRoll;
}

void BaseCameraController::setSpeedForward(float speedForward) {
    m_speedForward = speedForward;
}

void BaseCameraController::setSpeedBack(float speedBack) {
    m_speedBack = speedBack;
}

void BaseCameraController::setSpeedRight(float speedRight) {
    m_speedRight = speedRight;
}

void BaseCameraController::setSpeedLeft(float speedLeft) {
    m_speedLeft = speedLeft;
}

Camera* BaseCameraController::getCamera() const {
    return m_camera;
}

float BaseCameraController::getSensitivityPitch() const {
    return m_sensitivityPitch;
}

float BaseCameraController::getSensitivityYaw() const {
    return m_sensitivityYaw;
}

float BaseCameraController::getSensitivityRoll() const {
    return m_sensitivityRoll;
}

float BaseCameraController::getSpeedForward() const {
    return m_speedForward;
}

float BaseCameraController::getSpeedBack() const {
    return m_speedBack;
}

float BaseCameraController::getSpeedRight() const {
    return m_speedRight;
}

float BaseCameraController::getSpeedLeft() const {
    return m_speedLeft;
}


}

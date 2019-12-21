#define GLM_FORCE_CTOR_INIT
#include <algine/camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace algine {
Camera::Camera(const uint rotatorType): rotatable(rotatorType) {
    /* empty */
}

void Camera::translate() {
    m_translation = glm::translate(glm::mat4(), -m_pos);
}

void Camera::updateMatrix() {
    m_transform = m_rotation * m_translation * m_scaling;
}

void Camera::setFieldOfView(const float fov) {
    m_fov = fov;
}

void Camera::setAspectRatio(const float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

void Camera::setNear(const float near) {
    m_near = near;
}

void Camera::setFar(const float far) {
    m_far = far;
}

void Camera::perspective() {
    m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
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

void BaseCameraController::setMousePos(const float x, const float y, const float z) {
    lastMousePos = glm::vec3(x, y, z);
}

void BaseCameraController::mouseMove(const float x, const float y, const float z) {
    std::cerr << "You cannot call mouseMove(" << x << ", " << y << ", " << z << ") directly from BaseCameraController!\n";
}

void BaseCameraController::goForward() {
    camera->m_pos += camera->getForward() * speedf;
}

void BaseCameraController::goBack() {
    camera->m_pos += camera->getBack() * speedb;
}

void BaseCameraController::goRight() {
    camera->m_pos += camera->getRight() * speedr;
}

void BaseCameraController::goLeft() {
    camera->m_pos += camera->getLeft() * speedl;
}

void FreeCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;
    
    camera->setYaw(dCoord.x * sensitivityYaw);
    camera->setYaw(dCoord.y * sensitivityPitch);
    camera->setYaw(dCoord.z * sensitivityRoll);
 
    lastMousePos = glm::vec3(x, y, z);
}

void FPSCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;
    
    camera->rotator->yaw += dCoord.x * sensitivityYaw;
    camera->rotator->pitch += dCoord.y * sensitivityPitch;
    camera->rotator->roll += dCoord.z * sensitivityRoll;
 
    lastMousePos = glm::vec3(x, y, z);
}
}

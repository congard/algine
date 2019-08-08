#define GLM_FORCE_CTOR_INIT
#include <algine/camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace algine {
Camera::Camera(const uint rotatorType) : Object3D (rotatorType) {
    type = Object3DTypeCamera;
}

void Camera::translate() {
    m_translation = glm::translate(glm::mat4(), -v_pos);
}

void Camera::updateMatrix() {
    m_transform = m_rotation * m_translation * m_scaling;
}

void Camera::setFieldOfView(const float fov) {
    this->fov = fov;
}

void Camera::setAspectRatio(const float aspectRatio) {
    this->aspectRatio = aspectRatio;
}

void Camera::setNear(const float near) {
    this->near = near;
}

void Camera::setFar(const float far) {
    this->far = far;
}

void Camera::perspective() {
    m_projection = glm::perspective(fov, aspectRatio, near, far);
}

glm::mat4 Camera::getProjectionMatrix() {
    return m_projection;
}

glm::mat4 Camera::getViewMatrix() {
    return getMatrix();
}

float Camera::getFieldOfView() {
    return fov;
}

float Camera::getAspectRatio() {
    return aspectRatio;
}

float Camera::getNear() {
    return near;
}

float Camera::getFar() {
    return far;
}

void BaseCameraController::setMousePos(const float x, const float y, const float z) {
    lastMousePos = glm::vec3(x, y, z);
}

void BaseCameraController::mouseMove(const float x, const float y, const float z) {
    std::cerr << "You cannot call mouseMove(" << x << ", " << y << ", " << z << ") directly from BaseCameraController!\n";
}

void BaseCameraController::goForward() {
    camera->v_pos += camera->getForward() * speedf;
}

void BaseCameraController::goBack() {
    camera->v_pos += camera->getBack() * speedb;
}

void BaseCameraController::goRight() {
    camera->v_pos += camera->getRight() * speedr;
}

void BaseCameraController::goLeft() {
    camera->v_pos += camera->getLeft() * speedl;
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

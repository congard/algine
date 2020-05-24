#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/Camera.h>

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
}

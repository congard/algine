#define GLM_FORCE_CTOR_INIT
#include <algine/std/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace algine {
Camera::Camera(Type type)
    : m_projection(1.0f),
      m_transform(1.0f),
      m_fov(glm::radians(90.0f)),
      m_aspectRatio(1.0f),
      m_near(1.0f),
      m_far(32.0f)
{
    switch (type) {
        case Type::FPS: {
            setRotatorType(Rotator::Type::Euler);
            break;
        }
        case Type::Free: {
            setRotatorType(Rotator::Type::Free);
            break;
        }
    }
}

void Camera::translate() {
    m_translation = glm::translate(glm::mat4(1.0f), -m_pos);
}

void Camera::updateMatrix() {
    m_transform = m_rotation * m_translation * m_scaling;
}

inline void goCamera(Camera *camera, const glm::vec3 &dPos) {
    camera->changePos(dPos);
    camera->translate();
    camera->updateMatrix();
}

void Camera::goForward(float k) {
    goCamera(this, getForward() * k);
}

void Camera::goBack(float k) {
    goCamera(this, getBack() * k);
}

void Camera::goRight(float k) {
    goCamera(this, getRight() * k);
}

void Camera::goLeft(float k) {
    goCamera(this, getLeft() * k);
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

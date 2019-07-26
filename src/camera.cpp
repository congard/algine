#define GLM_FORCE_CTOR_INIT
#include <algine/camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace algine {
void BaseCamera::perspective() {
    projection = glm::perspective(fov, aspectRatio, near, far);
}

void Camera::lookAt() {
    view = glm::lookAt(pos, lookPos, up);
}

void FPSCamera::updateView() {
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
    glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
    glm::quat qRoll = glm::angleAxis(roll,glm::vec3(0, 0, 1));
    
    glm::quat orientation = qPitch * qYaw * qRoll;
    orientation = glm::normalize(orientation);
    glm::mat4 rotate = glm::mat4_cast(orientation);
 
    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -pos);
 
    view = rotate * translate;
}

void FreeCamera::updateView() {
    // temporary frame quaternion from pitch, yaw, roll
    glm::quat qPYR = glm::quat(glm::vec3(pitch, yaw, roll));
    // reset values
    pitch = yaw = roll = 0;
    
    // update qCamera
    qCamera = qPYR * qCamera;
    qCamera = glm::normalize(qCamera);
    glm::mat4 rotate = glm::mat4_cast(qCamera);
    
    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -pos);
    
    view = rotate * translate;
}

void BaseCameraController::goForward() {
    camera->pos -= glm::vec3(camera->view[0][2], camera->view[1][2], camera->view[2][2]) * speedf;
}

void BaseCameraController::goBack() {
    camera->pos += glm::vec3(camera->view[0][2], camera->view[1][2], camera->view[2][2]) * speedb;
}

void BaseCameraController::goRight() {
    camera->pos += glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]) * speedr;
}

void BaseCameraController::goLeft() {
    camera->pos -= glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]) * speedl;
}

void FPSCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;
    
    ((FPSCamera*)camera)->yaw += dCoord.x * sensitivityYaw;
    ((FPSCamera*)camera)->pitch += dCoord.y * sensitivityPitch;
    ((FPSCamera*)camera)->roll += dCoord.z * sensitivityRoll;
 
    lastMousePos = glm::vec3(x, y, z);
}

void FreeCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;
    
    ((FreeCamera*)camera)->yaw = dCoord.x * sensitivityYaw;
    ((FreeCamera*)camera)->pitch = dCoord.y * sensitivityPitch;
    ((FreeCamera*)camera)->roll = dCoord.z * sensitivityRoll;
 
    lastMousePos = glm::vec3(x, y, z);
}
}

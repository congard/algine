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
}

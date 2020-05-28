#define GLM_FORCE_CTOR_INIT
#include <algine/std/rotator/EulerRotator.h>

#include <glm/gtc/quaternion.hpp>

namespace algine {
EulerRotator::EulerRotator() {
    type = RotatorTypeEuler;
}

void EulerRotator::rotate(glm::mat4 &matrix) {
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat orientation = glm::normalize(qPitch * qYaw * qRoll);
    matrix = glm::mat4_cast(orientation);
}
}

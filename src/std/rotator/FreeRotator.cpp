#define GLM_FORCE_CTOR_INIT
#include <algine/std/rotator/FreeRotator.h>

namespace algine {
FreeRotator::FreeRotator() {
    type = RotatorTypeFree;
}

void FreeRotator::rotate(glm::mat4 &matrix) {
    // temporary frame quaternion from pitch, yaw, roll
    glm::quat qPYR = glm::quat(glm::vec3(pitch, yaw, roll));
    // reset values
    pitch = yaw = roll = 0;

    // update qRotation
    qRotation = glm::normalize(qPYR * qRotation);
    matrix = glm::mat4_cast(qRotation);
}
}

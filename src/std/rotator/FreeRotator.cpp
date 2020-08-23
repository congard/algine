#define GLM_FORCE_CTOR_INIT
#include <algine/std/rotator/FreeRotator.h>

using namespace glm;

namespace algine {
FreeRotator::FreeRotator() {
    m_type = RotatorTypeFree;
}

void FreeRotator::rotate(mat4 &matrix) {
    // temporary frame quaternion from pitch, yaw, roll
    quat qPYR = quat(vec3(m_pitch, m_yaw, m_roll));
    // reset values
    m_pitch = m_yaw = m_roll = 0;

    // update qRotation
    qRotation = normalize(qPYR * qRotation);
    matrix = mat4_cast(qRotation);
}
}

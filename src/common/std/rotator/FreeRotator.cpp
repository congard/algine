#include <algine/std/rotator/FreeRotator.h>

using namespace glm;

namespace algine {
FreeRotator::FreeRotator()
    : qRotation({1.0f, 1.0f, 1.0f})
{
    m_type = Type::Free;
}

void FreeRotator::rotate(mat4 &matrix) {
    // temporary frame quaternion from pitch, yaw, roll
    quat qPYR {{m_pitch, m_yaw, m_roll}};
    // reset values
    m_pitch = m_yaw = m_roll = 0;

    // update qRotation
    qRotation = normalize(qPYR * qRotation);
    matrix = mat4_cast(qRotation);
}

void FreeRotator::changeRotation(const vec3 &dRotate) {
    setPitch(dRotate.x);
    setYaw(dRotate.y);
    setRoll(dRotate.z);
}
}

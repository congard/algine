#include <algine/std/rotator/EulerRotator.h>

#include <glm/gtc/quaternion.hpp>

using namespace glm;

namespace algine {
EulerRotator::EulerRotator() {
    m_type = Type::Euler;
}

void EulerRotator::rotate(mat4 &matrix) {
    quat qPitch = angleAxis(m_pitch, vec3(1.0f, 0.0f, 0.0f));
    quat qYaw = angleAxis(m_yaw, vec3(0.0f, 1.0f, 0.0f));
    quat qRoll = angleAxis(m_roll, vec3(0.0f, 0.0f, 1.0f));

    quat orientation = normalize(qPitch * qYaw * qRoll);
    matrix = mat4_cast(orientation);
}
}

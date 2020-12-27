#include <algine/std/camera/FPSCamera.h>

namespace algine {
FPSCamera::FPSCamera()
    : Camera(Rotator::Type::Euler) {}

void FPSCamera::changeRotation(const glm::vec3 &dAngles) {
    m_rotator->changePitch(dAngles.x * m_sensitivityPitch);
    m_rotator->changeYaw(dAngles.y * m_sensitivityYaw);
    m_rotator->changeRoll(dAngles.z * m_sensitivityRoll);
}
}

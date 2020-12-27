#include <algine/std/camera/FreeCamera.h>

namespace algine {
FreeCamera::FreeCamera()
    : Camera(Rotator::Type::Free) {}

void FreeCamera::changeRotation(const glm::vec3 &dAngles) {
    setPitch(dAngles.x * m_sensitivityPitch);
    setYaw(dAngles.y * m_sensitivityYaw);
    setRoll(dAngles.z * m_sensitivityRoll);
}
}

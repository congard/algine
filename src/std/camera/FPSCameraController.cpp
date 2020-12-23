#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/FPSCameraController.h>

using namespace glm;

namespace algine {
void FPSCameraController::mouseMove(float x, float y, float z) {
    vec3 dCoord = vec3(x, y, z) - m_lastMousePos;

    auto rotator = m_camera->getRotator();

    rotator->changeYaw(dCoord.x * m_sensitivityYaw);
    rotator->changePitch(dCoord.y * m_sensitivityPitch);
    rotator->changeRoll(dCoord.z * m_sensitivityRoll);

    m_lastMousePos = {x, y, z};
}
}

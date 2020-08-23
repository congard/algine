#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/FPSCameraController.h>

using namespace glm;

namespace algine {
void FPSCameraController::mouseMove(float x, float y, float z) {
    vec3 dCoord = vec3(x, y, z) - lastMousePos;

    camera->rotator->m_yaw += dCoord.x * sensitivityYaw;
    camera->rotator->m_pitch += dCoord.y * sensitivityPitch;
    camera->rotator->m_roll += dCoord.z * sensitivityRoll;

    lastMousePos = vec3(x, y, z);
}
}

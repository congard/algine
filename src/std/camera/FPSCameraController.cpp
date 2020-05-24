#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/FPSCameraController.h>

namespace algine {
void FPSCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;

    camera->rotator->yaw += dCoord.x * sensitivityYaw;
    camera->rotator->pitch += dCoord.y * sensitivityPitch;
    camera->rotator->roll += dCoord.z * sensitivityRoll;

    lastMousePos = glm::vec3(x, y, z);
}
}

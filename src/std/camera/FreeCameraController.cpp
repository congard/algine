#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/FreeCameraController.h>

namespace algine {
void FreeCameraController::mouseMove(const float x, const float y, const float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - lastMousePos;

    camera->setYaw(dCoord.x * sensitivityYaw);
    camera->setYaw(dCoord.y * sensitivityPitch);
    camera->setYaw(dCoord.z * sensitivityRoll);

    lastMousePos = glm::vec3(x, y, z);
}
}

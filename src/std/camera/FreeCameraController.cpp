#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/FreeCameraController.h>

namespace algine {
void FreeCameraController::mouseMove(float x, float y, float z) {
    glm::vec3 dCoord = glm::vec3(x, y, z) - m_lastMousePos;

    m_camera->setYaw(dCoord.x * m_sensitivityYaw);
    m_camera->setPitch(dCoord.y * m_sensitivityPitch);
    m_camera->setRoll(dCoord.z * m_sensitivityRoll);

    m_lastMousePos = {x, y, z};
}
}

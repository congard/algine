#define GLM_FORCE_CTOR_INIT
#include <algine/std/camera/BaseCameraController.h>

namespace algine {
void BaseCameraController::setMousePos(const float x, const float y, const float z) {
    lastMousePos = glm::vec3(x, y, z);
}

void BaseCameraController::goForward() {
    camera->m_pos += camera->getForward() * speedf;
}

void BaseCameraController::goBack() {
    camera->m_pos += camera->getBack() * speedb;
}

void BaseCameraController::goRight() {
    camera->m_pos += camera->getRight() * speedr;
}

void BaseCameraController::goLeft() {
    camera->m_pos += camera->getLeft() * speedl;
}
}

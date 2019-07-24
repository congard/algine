#include <algine/camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace algine {
void BaseCamera::perspective() {
    projection = glm::perspective(fov, aspectRatio, near, far);
}

void Camera::lookAt() {
    view = glm::lookAt(pos, lookPos, up);
}
}

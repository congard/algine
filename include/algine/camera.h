#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace algine {
struct BaseCamera {
    glm::vec3 pos;
    glm::mat4 view, projection;
    float fov, aspectRatio, near = 1.0f, far = 32.0f;

    void perspective();
};

struct Camera : public BaseCamera {
    glm::vec3 lookPos, up = glm::vec3(0.0f, 1.0f, 0.0f);

    void lookAt();
};
}

#endif // ALGINE_CAMERA_H

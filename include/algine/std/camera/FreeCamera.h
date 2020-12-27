#ifndef ALGINE_FREECAMERA_H
#define ALGINE_FREECAMERA_H

#include <algine/std/camera/Camera.h>

namespace algine {
class FreeCamera: public Camera {
public:
    FreeCamera();

    void changeRotation(const glm::vec3 &dAngles) override;
};
}

#endif //ALGINE_FREECAMERA_H

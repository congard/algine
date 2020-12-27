#ifndef ALGINE_FPSCAMERA_H
#define ALGINE_FPSCAMERA_H

#include <algine/std/camera/Camera.h>

namespace algine {
class FPSCamera: public Camera {
public:
    FPSCamera();

    void changeRotation(const glm::vec3 &dAngles) override;
};
}


#endif //ALGINE_FPSCAMERA_H

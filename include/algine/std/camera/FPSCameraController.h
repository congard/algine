#ifndef ALGINE_FPSCAMERACONTROLLER_H
#define ALGINE_FPSCAMERACONTROLLER_H

#include <algine/std/camera/BaseCameraController.h>

namespace algine {
class FPSCameraController : public BaseCameraController {
public:
    void mouseMove(float x, float y, float z = 0) override;
};
}

#endif //ALGINE_FPSCAMERACONTROLLER_H

#ifndef ALGINE_FREECAMERACONTROLLER_H
#define ALGINE_FREECAMERACONTROLLER_H

#include <algine/std/camera/BaseCameraController.h>

namespace algine {
class FreeCameraController : public BaseCameraController {
public:
    void mouseMove(float x, float y, float z = 0) override;
};
}

#endif //ALGINE_FREECAMERACONTROLLER_H
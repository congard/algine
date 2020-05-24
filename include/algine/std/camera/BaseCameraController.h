#ifndef ALGINE_BASECAMERACONTROLLER_H
#define ALGINE_BASECAMERACONTROLLER_H

#include <algine/std/camera/Camera.h>
#include <glm/vec3.hpp>

namespace algine {
class BaseCameraController {
public:
    virtual void mouseMove(float x, float y, float z = 0) = 0;
    void setMousePos(float x, float y, float z = 0);
    void goForward();
    void goBack();
    void goRight();
    void goLeft();

public:
    Camera *camera;
    float sensitivityPitch = 0.0025f, sensitivityYaw = 0.0025f, sensitivityRoll = 0.0025f;
    float speedf = 1.0f, speedb = 1.0f, speedr = 1.0f, speedl = 1.0f;

protected:
    glm::vec3 lastMousePos = glm::vec3(0.0f);
};
}

#endif //ALGINE_BASECAMERACONTROLLER_H

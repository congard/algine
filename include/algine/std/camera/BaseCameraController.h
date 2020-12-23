#ifndef ALGINE_BASECAMERACONTROLLER_H
#define ALGINE_BASECAMERACONTROLLER_H

#include <algine/std/camera/Camera.h>

#include <glm/vec3.hpp>

// TODO: merge with Camera - eliminate Controller classes

namespace algine {
class BaseCameraController {
public:
    BaseCameraController();

    virtual void mouseMove(float x, float y, float z) = 0;
    void mouseMove(float x, float y);
    void setMousePos(float x, float y, float z = 0);
    void goForward();
    void goBack();
    void goRight();
    void goLeft();

    void setCamera(Camera *camera);

    void setSensitivityPitch(float sensitivityPitch);
    void setSensitivityYaw(float sensitivityYaw);
    void setSensitivityRoll(float sensitivityRoll);

    void setSpeedForward(float speedForward);
    void setSpeedBack(float speedBack);
    void setSpeedRight(float speedRight);
    void setSpeedLeft(float speedLeft);

    Camera* getCamera() const;

    float getSensitivityPitch() const;
    float getSensitivityYaw() const;
    float getSensitivityRoll() const;

    float getSpeedForward() const;
    float getSpeedBack() const;
    float getSpeedRight() const;
    float getSpeedLeft() const;

protected:
    Camera *m_camera;
    float m_sensitivityPitch, m_sensitivityYaw, m_sensitivityRoll;
    float m_speedForward, m_speedBack, m_speedRight, m_speedLeft;

protected:
    glm::vec3 m_lastMousePos;
};
}

#endif //ALGINE_BASECAMERACONTROLLER_H

#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

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

struct FreeCamera : public BaseCamera {
    float pitch = 0, yaw = 0, roll = 0;
    
    void updateView();
    
private:
    glm::quat qCamera;
};

struct FPSCamera : public FreeCamera {
    void updateView();
};

struct BaseCameraController {
    BaseCamera *camera;
    float speedf = 1.0f, speedb = 1.0f, speedr = 1.0f, speedl = 1.0f;
    
    void goForward();
    void goBack();
    void goRight();
    void goLeft();
};

struct FreeCameraController : public BaseCameraController {
    float sensitivityPitch = 0.0025f, sensitivityYaw = 0.0025f, sensitivityRoll = 0.0025f;
    
    void mouseMove(const float x, const float y, const float z = 0);
    
    inline void setMousePos(const float x, const float y, const float z = 0) {
        lastMousePos = glm::vec3(x, y, z);
    }
    
protected:
    glm::vec3 lastMousePos = glm::vec3(0.0f);
};

struct FPSCameraController : public FreeCameraController {
    void mouseMove(const float x, const float y, const float z = 0);
};
}

#endif // ALGINE_CAMERA_H

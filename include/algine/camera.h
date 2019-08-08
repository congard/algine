#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algine/object3d.h>

namespace algine {
class Camera: public Object3D {
public:
    glm::mat4 m_projection;
    float fov, aspectRatio, near = 1.0f, far = 32.0f;

    Camera(const uint rotatorType = Rotator::RotatorTypeEuler);

    void translate();
    void updateMatrix();
    void setFieldOfView(const float fov);
    void setAspectRatio(const float aspectRatio);
    void setNear(const float near);
    void setFar(const float far);
    void perspective();

    glm::mat4 getProjectionMatrix();
    glm::mat4 getViewMatrix(); // calls Object3D::getMatrix()
    float getFieldOfView();
    float getAspectRatio();
    float getNear();
    float getFar();
};

class BaseCameraController {
public:
    Camera *camera;
    float sensitivityPitch = 0.0025f, sensitivityYaw = 0.0025f, sensitivityRoll = 0.0025f;
    float speedf = 1.0f, speedb = 1.0f, speedr = 1.0f, speedl = 1.0f;
    
    void setMousePos(const float x, const float y, const float z = 0);
    void mouseMove(const float x, const float y, const float z = 0);
    void goForward();
    void goBack();
    void goRight();
    void goLeft();

protected:
    glm::vec3 lastMousePos = glm::vec3(0.0f);
};

class FreeCameraController : public BaseCameraController {
public:
    void mouseMove(const float x, const float y, const float z = 0);
};

class FPSCameraController : public BaseCameraController {
public:
    void mouseMove(const float x, const float y, const float z = 0);
};
}

#endif // ALGINE_CAMERA_H

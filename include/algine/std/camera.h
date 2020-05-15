#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algine/std/object3d.h>

namespace algine {
class Camera: public rotatable, public translatable, public scalable {
public:
    explicit Camera(uint rotatorType = Rotator::RotatorTypeEuler);

    void translate() override;
    void updateMatrix();
    void setFieldOfView(float fov);
    void setAspectRatio(float aspectRatio);
    void setNear(float near);
    void setFar(float far);
    void perspective();

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    float getFieldOfView() const;
    float getAspectRatio() const;
    float getNear() const;
    float getFar() const;

public:
    glm::mat4 m_projection, m_transform; // m_transform is view matrix
    float m_fov = 1.5708f, // 90 degrees
          m_aspectRatio = 1.0f,
          m_near = 1.0f,
          m_far = 32.0f;
};

class BaseCameraController {
public:
    Camera *camera;
    float sensitivityPitch = 0.0025f, sensitivityYaw = 0.0025f, sensitivityRoll = 0.0025f;
    float speedf = 1.0f, speedb = 1.0f, speedr = 1.0f, speedl = 1.0f;
    
    void setMousePos(float x, float y, float z = 0);
    void mouseMove(float x, float y, float z = 0);
    void goForward();
    void goBack();
    void goRight();
    void goLeft();

protected:
    glm::vec3 lastMousePos = glm::vec3(0.0f);
};

class FreeCameraController : public BaseCameraController {
public:
    void mouseMove(float x, float y, float z = 0);
};

class FPSCameraController : public BaseCameraController {
public:
    void mouseMove(float x, float y, float z = 0);
};
}

#endif // ALGINE_CAMERA_H

#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <algine/std/object3d.h>
#include <glm/mat4x4.hpp>

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
}

#endif //ALGINE_CAMERA_H

#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <algine/std/Rotatable.h>
#include <algine/std/Translatable.h>
#include <algine/std/Scalable.h>

#include <glm/mat4x4.hpp>

namespace algine {
class Camera: public Rotatable, public Translatable, public Scalable {
public:
    virtual void changeRotation(const glm::vec3 &dAngles) = 0;

    void translate() override;
    void updateMatrix();

    void goForward();
    void goBack();
    void goRight();
    void goLeft();

    void setFieldOfView(float fov);
    void setAspectRatio(float aspectRatio);
    void setNear(float near);
    void setFar(float far);
    void perspective();

    void setSensitivityPitch(float sensitivityPitch);
    void setSensitivityYaw(float sensitivityYaw);
    void setSensitivityRoll(float sensitivityRoll);

    void setSpeedForward(float speedForward);
    void setSpeedBack(float speedBack);
    void setSpeedRight(float speedRight);
    void setSpeedLeft(float speedLeft);

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;
    float getFieldOfView() const;
    float getAspectRatio() const;
    float getNear() const;
    float getFar() const;

    float getSensitivityPitch() const;
    float getSensitivityYaw() const;
    float getSensitivityRoll() const;

    float getSpeedForward() const;
    float getSpeedBack() const;
    float getSpeedRight() const;
    float getSpeedLeft() const;

protected:
    explicit Camera(Rotator::Type rotatorType);

protected:
    glm::mat4 m_projection, m_transform; // m_transform is view matrix
    float m_fov, m_aspectRatio, m_near, m_far;
    float m_sensitivityPitch, m_sensitivityYaw, m_sensitivityRoll;
    float m_speedForward, m_speedBack, m_speedRight, m_speedLeft;

private:
    using Rotatable::setRotatorType;
};
}

#endif //ALGINE_CAMERA_H

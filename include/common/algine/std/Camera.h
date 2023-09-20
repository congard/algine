#ifndef ALGINE_CAMERA_H
#define ALGINE_CAMERA_H

#include <algine/std/Rotatable.h>
#include <algine/std/Translatable.h>
#include <algine/std/Scalable.h>

#include <glm/mat4x4.hpp>

namespace algine {
class AL_EXPORT Camera: public Rotatable, public Translatable, public Scalable {
public:
    enum class Type {
        FPS,
        Free
    };

public:
    explicit Camera(Type type = Type::FPS);

    void translate() override;
    void updateMatrix();

    void goForward(float k = 1.0f);
    void goBack(float k = 1.0f);
    void goRight(float k = 1.0f);
    void goLeft(float k = 1.0f);

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

protected:
    glm::mat4 m_projection, m_transform; // m_transform is view matrix
    float m_fov, m_aspectRatio, m_near, m_far;

private:
    using Rotatable::setRotatorType;
};
}

#endif //ALGINE_CAMERA_H

#ifndef ALGINE_OBJECT3D_H
#define ALGINE_OBJECT3D_H

#include <algine/rotator.h>
#include <algine/types.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace algine {
class Object3D {
private:
    void swap(Object3D &other);
public:
    enum {
        Object3DTypePlain,
        Object3DTypeModel,
        Object3DTypeCamera
    };

    Rotator *rotator = nullptr;
    glm::mat4 m_rotation, m_translation, m_scaling;
    glm::mat4 m_transform;
    glm::vec3 v_pos, v_scale;
    uint type = Object3DTypePlain;

    explicit Object3D(uint rotatorType = Rotator::RotatorTypeSimple);
    Object3D(const Object3D &src); // copy constructor
    Object3D(Object3D &&src) noexcept; // move constructor
    Object3D& operator=(const Object3D &rhs); // copy operator
    Object3D& operator=(Object3D &&rhs) noexcept; // move operator
    ~Object3D();

    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);
    void setRotate(float pitch, float yaw, float roll);
    void rotate();
    void setPos(float x, float y, float z);
    void setPos(const glm::vec3 &pos);
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void translate();
    void setScale(float x, float y, float z);
    void setScale(const glm::vec3 &scale);
    void setScaleX(float x);
    void setScaleY(float y);
    void setScaleZ(float z);
    void scale();
    void updateMatrix();

    glm::vec3 getPosition();
    float getX();
    float getY();
    float getZ();
    glm::vec3 getScale();
    float getScaleX();
    float getScaleY();
    float getScaleZ();
    glm::vec3 getRotate();
    float getPitch();
    float getYaw();
    float getRoll();
    glm::mat4 getRotationMatrix();
    glm::mat4 getTranslationMatrix();
    glm::mat4 getScalingMatrix();
    glm::mat4 getMatrix();
    glm::vec3 getForward();
    glm::vec3 getBack();
    glm::vec3 getRight();
    glm::vec3 getLeft();
};
}

#endif // ALGINE_OBJECT3D_H
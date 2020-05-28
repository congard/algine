#ifndef ALGINE_OBJECT3D_H
#define ALGINE_OBJECT3D_H

#include <algine/std/rotator/Rotator.h>
#include <algine/types.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace algine {
/**
 * Property. Implements functions for rotating an object
 */
class rotatable {
protected:
    void swap(rotatable &other);

public:
    explicit rotatable(uint rotatorType = Rotator::RotatorTypeSimple);
    rotatable(const rotatable &src); // copy constructor
    rotatable(rotatable &&src) noexcept; // move constructor
    rotatable& operator=(const rotatable &rhs); // copy operator
    rotatable& operator=(rotatable &&rhs) noexcept; // move operator
    ~rotatable();

    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);
    void setRotate(float pitch, float yaw, float roll);
    void rotate();

    glm::vec3 getRotate() const;
    float getPitch() const;
    float getYaw() const;
    float getRoll() const;
    glm::mat4 getRotationMatrix() const;
    glm::vec3 getForward() const;
    glm::vec3 getBack() const;
    glm::vec3 getRight() const;
    glm::vec3 getLeft() const;

public:
    Rotator *rotator = nullptr;
    glm::mat4 m_rotation;
};

/**
 * Property. Implements functions for translating an object
 */
class translatable {
public:
    void setPos(float x, float y, float z);
    void setPos(const glm::vec3 &pos);
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    virtual void translate();

    glm::vec3 getPos() const;
    float getX() const;
    float getY() const;
    float getZ() const;
    glm::mat4 getTranslationMatrix() const;

public:
    glm::vec3 m_pos;
    glm::mat4 m_translation;
};

/**
 * Property. Implements functions for scaling an object
 */
class scalable {
public:
    void setScale(float x, float y, float z);
    void setScale(const glm::vec3 &scale);
    void setScaleX(float x);
    void setScaleY(float y);
    void setScaleZ(float z);
    void scale();

    glm::vec3 getScale() const;
    float getScaleX() const;
    float getScaleY() const;
    float getScaleZ() const;
    glm::mat4 getScalingMatrix() const;

public:
    glm::vec3 m_scale;
    glm::mat4 m_scaling;
};
}

#endif // ALGINE_OBJECT3D_H

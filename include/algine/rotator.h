#ifndef ALGINE_ROTATOR_H
#define ALGINE_ROTATOR_H

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algine/types.h>

namespace algine {
class Rotator {
public:
    enum {
        RotatorTypeSimple,
        RotatorTypeEuler,
        RotatorTypeFree
    };

    float pitch = 0, yaw = 0, roll = 0;
    uint type = RotatorTypeSimple;

    virtual ~Rotator() = default; // to hide -Wdelete-non-virtual-dtor

    virtual void rotate(glm::mat4 &matrix);
    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);

    float getPitch();
    float getYaw();
    float getRoll();

    static Rotator* create(uint type);
};

class EulerRotator: public Rotator {
public:
    EulerRotator();

    void rotate(glm::mat4 &matrix) override;
};

class FreeRotator: public Rotator {
public:
    glm::quat qRotation;

    FreeRotator();

    void rotate(glm::mat4 &matrix) override;
};
}

#endif // ALGINE_ROTATOR_H

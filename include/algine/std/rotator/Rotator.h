#ifndef ALGINE_ROTATOR_H
#define ALGINE_ROTATOR_H

#include <algine/core/transfer/Transferable.h>

#include <algine/types.h>

#include <glm/mat4x4.hpp>

namespace algine {
class Rotator: public Transferable {
public:
    enum class Type {
        Simple,
        Euler,
        Free
    };

public:
    Rotator();
    virtual ~Rotator() = default; // to hide -Wdelete-non-virtual-dtor

    virtual void rotate(glm::mat4 &matrix);

    void changePitch(float dPitch);
    void changeYaw(float dYaw);
    void changeRoll(float dRoll);

    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);

    float getPitch() const;
    float getYaw() const;
    float getRoll() const;

    Type getType() const;

    static Rotator* create(Type type);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    float m_pitch, m_yaw, m_roll;
    Type m_type;
};
}

#endif //ALGINE_ROTATOR_H

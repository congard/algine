#ifndef ALGINE_ROTATOR_H
#define ALGINE_ROTATOR_H

#include <algine/core/lua/Lua.h>

#include <glm/mat4x4.hpp>

namespace algine {
class AL_EXPORT Rotator {
public:
    enum class Type {
        Simple,
        Euler,
        Free
    };

public:
    Rotator();
    virtual ~Rotator() = default;

    virtual void rotate(glm::mat4 &matrix);
    virtual void changeRotation(const glm::vec3 &dRotate);

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

protected:
    float m_pitch, m_yaw, m_roll;
    Type m_type;
};
}

#endif //ALGINE_ROTATOR_H

#ifndef ALGINE_ROTATOR_H
#define ALGINE_ROTATOR_H

#include <glm/mat4x4.hpp>
#include <algine/types.h>

#include <algine/core/transfer/Transferable.h>

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
    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);

    float getPitch() const;
    float getYaw() const;
    float getRoll() const;

    static Rotator* create(Type type);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    float m_pitch, m_yaw, m_roll;
    Type m_type;
};
}

#endif //ALGINE_ROTATOR_H

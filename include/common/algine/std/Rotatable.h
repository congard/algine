#ifndef ALGINE_ROTATABLE_H
#define ALGINE_ROTATABLE_H

#include <algine/std/rotator/Rotator.h>
#include <algine/core/lua/Scriptable.h>

namespace algine {
class Rotatable: public virtual Scriptable {
protected:
    void swap(Rotatable &other);

public:
    explicit Rotatable(Rotator::Type rotatorType);
    Rotatable();
    Rotatable(const Rotatable &src); // copy constructor
    Rotatable(Rotatable &&src) noexcept; // move constructor
    Rotatable& operator=(const Rotatable &rhs); // copy operator
    Rotatable& operator=(Rotatable &&rhs) noexcept; // move operator
    ~Rotatable();

    void setRotatorType(Rotator::Type rotatorType);

    void setPitch(float pitch);
    void setYaw(float yaw);
    void setRoll(float roll);
    void setRotate(float pitch, float yaw, float roll);

    void changeRotation(const glm::vec3 &dRotate);

    void rotate();

    float getPitch() const;
    float getYaw() const;
    float getRoll() const;
    const glm::mat4& getRotationMatrix() const;
    glm::vec3 getRotate() const;
    glm::vec3 getForward() const;
    glm::vec3 getBack() const;
    glm::vec3 getRight() const;
    glm::vec3 getLeft() const;
    Rotator* getRotator() const;

    static void registerLuaUsertype(Lua *lua);

protected:
    Rotator *m_rotator;
    glm::mat4 m_rotation;
};
}

#endif //ALGINE_ROTATABLE_H

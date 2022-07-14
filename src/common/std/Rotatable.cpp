#include <algine/std/Rotatable.h>

#include <tulz/macros.h>

#include <algorithm>

using namespace glm;

namespace algine {
void Rotatable::swap(Rotatable &other) {
    std::swap(m_rotation, other.m_rotation);
    std::swap(m_rotator, other.m_rotator);
}

Rotatable::Rotatable(Rotator::Type rotatorType)
    : m_rotator(Rotator::create(rotatorType)),
      m_rotation(1.0f) {}

Rotatable::Rotatable()
    : m_rotator(nullptr),
      m_rotation(1.0f) {}

Rotatable::Rotatable(const Rotatable &src)
    : m_rotation(src.m_rotation),
      m_rotator(Rotator::create(src.m_rotator->getType())) {}

Rotatable::Rotatable(Rotatable &&src) noexcept {
    src.swap(*this);
}

Rotatable& Rotatable::operator=(const Rotatable &rhs) {
    if (&rhs != this) {
        Rotatable(rhs).swap(*this);
    }

    return *this;
}

Rotatable& Rotatable::operator=(Rotatable &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

Rotatable::~Rotatable() {
    deletePtr(m_rotator)
}

void Rotatable::setRotatorType(Rotator::Type rotatorType) {
    delete m_rotator;
    m_rotator = Rotator::create(rotatorType);
}

void Rotatable::setPitch(float pitch) {
    m_rotator->setPitch(pitch);
}

void Rotatable::setYaw(float yaw) {
    m_rotator->setYaw(yaw);
}

void Rotatable::setRoll(float roll) {
    m_rotator->setRoll(roll);
}

void Rotatable::setRotate(float pitch, float yaw, float roll) {
    setPitch(pitch);
    setYaw(yaw);
    setRoll(roll);
}

void Rotatable::changeRotation(const vec3 &dRotate) {
    m_rotator->changeRotation(dRotate);
}

void Rotatable::rotate() {
    m_rotator->rotate(m_rotation);
}

float Rotatable::getPitch() const {
    return m_rotator->getPitch();
}

float Rotatable::getYaw() const {
    return m_rotator->getYaw();
}

float Rotatable::getRoll() const {
    return m_rotator->getRoll();
}

const mat4& Rotatable::getRotationMatrix() const {
    return m_rotation;
}

vec3 Rotatable::getRotate() const {
    return {
        getPitch(),
        getYaw(),
        getRoll()
    };
}

vec3 Rotatable::getForward() const {
    return -getBack();
}

vec3 Rotatable::getBack() const {
    return {
        m_rotation[0][2],
        m_rotation[1][2],
        m_rotation[2][2]
    };
}

vec3 Rotatable::getRight() const {
    return {
        m_rotation[0][0],
        m_rotation[1][0],
        m_rotation[2][0]
    };
}

vec3 Rotatable::getLeft() const {
    return -getRight();
}

Rotator* Rotatable::getRotator() const {
    return m_rotator;
}

void Rotatable::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Rotatable"))
        return;

    lua->registerUsertype<Scriptable>();

    auto usertype = lua->state()->new_usertype<Rotatable>(
            "Rotatable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable>());

    usertype["setRotatorType"] = &Rotatable::setRotatorType;
    usertype["setRotate"] = &Rotatable::setRotate;
    usertype["changeRotation"] = &Rotatable::changeRotation;
    usertype["rotate"] = &Rotatable::rotate;

    Lua::new_property(usertype, "pitch", &Rotatable::getPitch, &Rotatable::setPitch);
    Lua::new_property(usertype, "yaw", &Rotatable::getYaw, &Rotatable::setYaw);
    Lua::new_property(usertype, "roll", &Rotatable::getRoll, &Rotatable::setRoll);

    usertype["getRotationMatrix"] = &Rotatable::getRotationMatrix;
    usertype["getRotate"] = &Rotatable::getRotate;
    usertype["getForward"] = &Rotatable::getForward;
    usertype["getBack"] = &Rotatable::getBack;
    usertype["getRight"] = &Rotatable::getRight;
    usertype["getLeft"] = &Rotatable::getLeft;
    usertype["getRotator"] = &Rotatable::getRotator;
}
}

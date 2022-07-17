#include <algine/std/rotator/Rotator.h>
#include <algine/std/rotator/EulerRotator.h>
#include <algine/std/rotator/FreeRotator.h>

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

using namespace std;
using namespace glm;

namespace algine {
Rotator::Rotator()
    : m_pitch(0),
      m_yaw(0),
      m_roll(0),
      m_type(Type::Simple) {}

void Rotator::rotate(mat4 &matrix) {
    matrix = glm::rotate(matrix, m_pitch, {1.0f, 0.0f, 0.0f});
    matrix = glm::rotate(matrix, m_yaw, {0.0f, 1.0f, 0.0f});
    matrix = glm::rotate(matrix, m_roll, {0.0f, 0.0f, 1.0f});
}

void Rotator::changeRotation(const vec3 &dRotate) {
    changePitch(dRotate.x);
    changeYaw(dRotate.y);
    changeRoll(dRotate.z);
}

void Rotator::changePitch(float dPitch) {
    m_pitch += dPitch;
}

void Rotator::changeYaw(float dYaw) {
    m_yaw += dYaw;
}

void Rotator::changeRoll(float dRoll) {
    m_roll += dRoll;
}

void Rotator::setPitch(float pitch) {
    m_pitch = pitch;
}

void Rotator::setYaw(float yaw) {
    m_yaw = yaw;
}

void Rotator::setRoll(float roll) {
    m_roll = roll;
}

float Rotator::getPitch() const {
    return m_pitch;
}

float Rotator::getYaw() const {
    return m_yaw;
}

float Rotator::getRoll() const {
    return m_roll;
}

Rotator::Type Rotator::getType() const {
    return m_type;
}

Rotator* Rotator::create(Type type) {
    switch (type) {
        case Type::Simple: {
            return new Rotator();
        }
        case Type::Euler: {
            return new EulerRotator();
        }
        case Type::Free: {
            return new FreeRotator();
        }
        default: {
            throw runtime_error("Unknown rotator type " + to_string(static_cast<int>(type)));
        }
    }
}

void Rotator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = Lua::getEnv(lua, tenv);

    if (Lua::isRegistered(env, "Rotator"))
        return;

    lua->registerUsertype<EulerRotator, FreeRotator>(tenv);

    auto usertype = env.new_usertype<Rotator>(
            "Rotator",
            sol::meta_function::construct, sol::default_constructor,
            sol::call_constructor, sol::default_constructor);

    usertype["rotate"] = &Rotator::rotate;
    usertype["changeRotation"] = &Rotator::changeRotation;
    usertype["changePitch"] = &Rotator::changePitch;
    usertype["changeYaw"] = &Rotator::changeYaw;
    usertype["changeRoll"] = &Rotator::changeRoll;

    Lua::new_property(usertype, "pitch", &Rotator::getPitch, &Rotator::setPitch);
    Lua::new_property(usertype, "yaw", &Rotator::getYaw, &Rotator::setYaw);
    Lua::new_property(usertype, "roll", &Rotator::getRoll, &Rotator::setRoll);

    usertype["getType"] = &Rotator::getType;

    // static
    usertype["create"] = &Rotator::create;

    env["Rotator"].get<sol::table>().new_enum("Type",
        "Simple", Type::Simple,
        "Euler", Type::Euler,
        "Free", Type::Free);
}
}

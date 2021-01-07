#define GLM_FORCE_CTOR_INIT
#include <algine/std/rotator/Rotator.h>

#include <algine/std/rotator/EulerRotator.h>
#include <algine/std/rotator/FreeRotator.h>

#include <algine/core/JsonHelper.h>

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

using namespace std;
using namespace glm;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Type, "type");
constant(Yaw, "yaw");
constant(Pitch, "pitch");
constant(Roll, "roll");

namespace RotatorType {
constant(Simple, "simple");
constant(Euler, "euler");
constant(Free, "free");
}
}

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

void Rotator::import(const JsonHelper &jsonHelper) {
    using namespace Config::RotatorType;

    const json &config = jsonHelper.json;

    jsonHelper.readValue(Config::Yaw, m_yaw);
    jsonHelper.readValue(Config::Pitch, m_pitch);
    jsonHelper.readValue(Config::Roll, m_roll);

    string type_str;
    jsonHelper.readValue(Config::Type, type_str);

    map<string, Type> types {
        {Simple, Type::Simple},
        {Euler, Type::Euler},
        {Free, Type::Free}
    };

    if (types.find(type_str) != types.end()) {
        m_type = types[type_str];
    } else {
        throw runtime_error(
            "Unknown Rotator type: " + type_str + "\n"
            "Available: " + Simple + ", " + Euler + ", " + Free
        );
    }
}

JsonHelper Rotator::dump() {
    using namespace Config::RotatorType;

    json config;

    config[Config::Yaw] = m_yaw;
    config[Config::Pitch] = m_pitch;
    config[Config::Roll] = m_roll;
    config[Config::Type] = (vector<string> {Simple, Euler, Free})[static_cast<uint>(m_type)];

    return config;
}
}

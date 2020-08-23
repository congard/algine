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
void Rotator::rotate(mat4 &matrix) {
    matrix = glm::rotate(matrix, m_pitch, vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, m_yaw, vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, m_roll, vec3(0.0f, 0.0f, 1.0f));
}

void Rotator::setPitch(const float pitch) {
    m_pitch = pitch;
}

void Rotator::setYaw(const float yaw) {
    m_yaw = yaw;
}

void Rotator::setRoll(const float roll) {
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

Rotator* Rotator::create(uint type) {
    switch (type) {
        case RotatorTypeSimple:
            return new Rotator();
        case RotatorTypeEuler:
            return new EulerRotator();
        case RotatorTypeFree:
            return new FreeRotator();
        default:
            throw runtime_error("Unknown rotator type " + to_string(type));
    }
}

void Rotator::import(const JsonHelper &jsonHelper) {
    using namespace Config;
    using namespace Config::RotatorType;

    const json &config = jsonHelper.json;

    jsonHelper.readValue(Yaw, m_yaw);
    jsonHelper.readValue(Pitch, m_pitch);
    jsonHelper.readValue(Roll, m_roll);

    string type_str;
    jsonHelper.readValue(Type, type_str);

    map<string, int> types {
        {Simple, RotatorTypeSimple},
        {Euler,  RotatorTypeEuler},
        {Free,   RotatorTypeFree}
    };

    if (types.find(type_str) != types.end()) {
        m_type = types[type_str];
    } else {
        string message = "Unknown Rotator type: " + type_str + "\n"
                          "Available: " + Simple + ", " + Euler + ", " + Free;
        throw runtime_error(message);
    }
}

JsonHelper Rotator::dump() {
    using namespace Config;
    using namespace Config::RotatorType;

    json config;

    config[Yaw] = m_yaw;
    config[Pitch] = m_pitch;
    config[Roll] = m_roll;
    config[Type] = (vector<string> {Simple, Euler, Free})[m_type];

    return config;
}
}

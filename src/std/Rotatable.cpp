#define GLM_FORCE_CTOR_INIT
#include <algine/std/Rotatable.h>

#include <algine/core/JsonHelper.h>
#include <algine/core/transfer/GLMTransferrer.h>

#include <tulz/macros.h>
#include <algorithm>

using namespace glm;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Rotation, "rotation");
constant(Rotator, "rotator");
}

namespace algine {
void Rotatable::swap(Rotatable &other) {
    std::swap(m_rotation, other.m_rotation);
    std::swap(rotator, other.rotator);
}

Rotatable::Rotatable(Rotator::Type rotatorType) {
    rotator = Rotator::create(rotatorType);
}

Rotatable::Rotatable(const Rotatable &src) {
    m_rotation = src.m_rotation;
    rotator = Rotator::create(src.rotator->m_type);
}

Rotatable::Rotatable(Rotatable &&src) noexcept {
    src.swap(*this);
}

Rotatable& Rotatable::operator=(const Rotatable &rhs) {
    if (&rhs != this)
        Rotatable(rhs).swap(*this);
    return *this;
}

Rotatable& Rotatable::operator=(Rotatable &&rhs) noexcept {
    rhs.swap(*this);
    return *this;
}

Rotatable::~Rotatable() {
    deletePtr(rotator)
}

void Rotatable::setPitch(const float pitch) {
    rotator->m_pitch = pitch;
}

void Rotatable::setYaw(const float yaw) {
    rotator->m_yaw = yaw;
}

void Rotatable::setRoll(const float roll) {
    rotator->m_roll = roll;
}

void Rotatable::setRotate(const float pitch, const float yaw, const float roll) {
    rotator->m_pitch = pitch;
    rotator->m_yaw = yaw;
    rotator->m_roll = roll;
}

void Rotatable::rotate() {
    rotator->rotate(m_rotation);
}

float Rotatable::getPitch() const {
    return rotator->m_pitch;
}

float Rotatable::getYaw() const {
    return rotator->m_yaw;
}

float Rotatable::getRoll() const {
    return rotator->m_roll;
}

mat4 Rotatable::getRotationMatrix() const {
    return m_rotation;
}

vec3 Rotatable::getRotate() const {
    return {
        rotator->m_pitch,
        rotator->m_yaw,
        rotator->m_roll
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

JsonHelper Rotatable::dump() {
    json config;

    config[Config::Rotation] = GLMTransferrer::dump(m_rotation).json;
    config[Config::Rotator] = rotator->dump().json;

    return config;
}

void Rotatable::import(const JsonHelper &jsonHelper) {
    m_rotation = GLMTransferrer::import<mat4>(jsonHelper.readValue(Config::Rotation));
    rotator->import(jsonHelper.readValue(Config::Rotator));
}
}

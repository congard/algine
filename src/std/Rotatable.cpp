#define GLM_FORCE_CTOR_INIT
#include <algine/std/Rotatable.h>

#include <algine/core/transfer/GLMTransferrer.h>
#include <algine/core/JsonHelper.h>

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
    std::swap(m_rotator, other.m_rotator);
}

Rotatable::Rotatable(Rotator::Type rotatorType)
    : m_rotator(Rotator::create(rotatorType)) {}

Rotatable::Rotatable()
    : m_rotator(nullptr) {}

Rotatable::Rotatable(const Rotatable &src)
    : m_rotation(src.m_rotation),
      m_rotator(Rotator::create(src.m_rotator->getType())) {}

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

mat4 Rotatable::getRotationMatrix() const {
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

JsonHelper Rotatable::dump() {
    json config;

    config[Config::Rotation] = GLMTransferrer::dump(m_rotation).json;
    config[Config::Rotator] = m_rotator->dump().json;

    return config;
}

void Rotatable::import(const JsonHelper &jsonHelper) {
    m_rotation = GLMTransferrer::import<mat4>(jsonHelper.readValue(Config::Rotation));
    m_rotator->import(jsonHelper.readValue(Config::Rotator));
}
}

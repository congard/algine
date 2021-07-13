#include <algine/std/Scalable.h>

#include <algine/core/JsonHelper.h>
#include <algine/core/transfer/GLMTransferrer.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Scaling, "scaling");
constant(Scale, "scale");
}

namespace algine {
Scalable::Scalable()
    : m_scale(1.0f),
      m_scaling(1.0f) {}

void Scalable::changeScale(const vec3 &dScale) {
    m_scale += dScale;
}

void Scalable::setScale(float x, float y, float z) {
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

void Scalable::setScale(const vec3 &scale) {
    m_scale = scale;
}

void Scalable::setScaleX(float x) {
    m_scale.x = x;
}

void Scalable::setScaleY(float y) {
    m_scale.y = y;
}

void Scalable::setScaleZ(float z) {
    m_scale.z = z;
}

void Scalable::scale() {
    m_scaling = glm::scale(glm::mat4(1.0f), m_scale);
}

float Scalable::getScaleX() const {
    return m_scale.x;
}

float Scalable::getScaleY() const {
    return m_scale.y;
}

float Scalable::getScaleZ() const {
    return m_scale.z;
}

const vec3& Scalable::getScale() const {
    return m_scale;
}

const glm::mat4& Scalable::getScalingMatrix() const {
    return m_scaling;
}

void Scalable::import(const JsonHelper &jsonHelper) {
    m_scaling = GLMTransferrer::import<mat4>(jsonHelper.readValue(Config::Scaling));
    m_scale = GLMTransferrer::import<vec3>(jsonHelper.readValue(Config::Scale));
}

JsonHelper Scalable::dump() {
    json config;

    config[Config::Scaling] = GLMTransferrer::dump(m_scaling).json;
    config[Config::Scale] = GLMTransferrer::dump(m_scale).json;

    return config;
}
}

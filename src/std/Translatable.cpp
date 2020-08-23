#define GLM_FORCE_CTOR_INIT
#include <algine/std/Translatable.h>

#include <algine/core/transfer/GLMTransferrer.h>
#include <algine/core/JsonHelper.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace nlohmann;
using namespace glm;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Translation, "translation");
constant(Pos, "pos");
}

namespace algine {
void Translatable::setPos(const float x, const float y, const float z) {
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void Translatable::setPos(const vec3 &pos) {
    m_pos = pos;
}

void Translatable::setX(const float x) {
    m_pos.x = x;
}

void Translatable::setY(const float y) {
    m_pos.y = y;
}

void Translatable::setZ(const float z) {
    m_pos.z = z;
}

void Translatable::translate() {
    m_translation = glm::translate(mat4(), m_pos);
}

float Translatable::getX() const {
    return m_pos.x;
}

float Translatable::getY() const {
    return m_pos.y;
}

float Translatable::getZ() const {
    return m_pos.z;
}

vec3 Translatable::getPos() const {
    return m_pos;
}

mat4 Translatable::getTranslationMatrix() const {
    return m_translation;
}

void Translatable::import(const JsonHelper &jsonHelper) {
    m_translation = GLMTransferrer::import<mat4>(jsonHelper.readValue(Config::Translation));
    m_pos = GLMTransferrer::import<vec3>(jsonHelper.readValue(Config::Pos));
}

JsonHelper Translatable::dump() {
    json config;

    config[Config::Translation] = GLMTransferrer::dump(m_translation).json;
    config[Config::Pos] = GLMTransferrer::dump(m_pos).json;

    return config;
}
}

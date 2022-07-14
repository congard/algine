#include <algine/std/Scalable.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

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

void Scalable::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Scalable"))
        return;

    lua->registerUsertype<Scriptable>();

    auto usertype = lua->state()->new_usertype<Scalable>(
            "Scalable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<Scriptable>());

    Lua::new_property(usertype, "scale", &Scalable::getScale, static_cast<void (Scalable::*)(const glm::vec3&)>(&Scalable::setScale));
    Lua::new_property(usertype, "scaleX", &Scalable::getScaleX, &Scalable::setScaleX);
    Lua::new_property(usertype, "scaleY", &Scalable::getScaleY, &Scalable::setScaleY);
    Lua::new_property(usertype, "scaleZ", &Scalable::getScaleZ, &Scalable::setScaleZ);

    usertype["changeScale"] = &Scalable::changeScale;
    usertype["scale"] = &Scalable::scale;
    usertype["getScalingMatrix"] = &Scalable::getScalingMatrix;
}
}

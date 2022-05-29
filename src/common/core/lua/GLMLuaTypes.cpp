#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/Engine.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm_string_cast.h"

namespace algine {
template<glm::length_t L>
auto multiply(const sol::object &p1, const sol::object &p2) {
    auto state = p1.lua_state();

    using S = float;
    using V = glm::vec<L, S>;
    using M = glm::mat<L, L, S>;
    using Q = glm::qua<S>;

    if (p1.is<V>()) { // if p1 is vector, then p2 can be T or M
        if (p2.is<M>()) {
            return sol::make_object(state, p1.as<V>() * p2.as<M>()); // vector
        } else if (p2.is<V>()) {
            return sol::make_object(state, p1.as<V>() * p2.as<V>()); // scalar
        } else if (p2.is<S>()) {
            return sol::make_object(state, p1.as<V>() * p2.as<S>()); // vector
        } else if (p2.is<Q>()) {
            if constexpr(L > 2) {
                return sol::make_object(state, p1.as<V>() * p2.as<Q>()); // vector
            }
        }
    } else if (p1.is<M>()) { // if p1 is matrix, then p2 can be M, V or S
        if (p2.is<M>()) {
            return sol::make_object(state, p1.as<M>() * p2.as<M>()); // matrix
        } else if (p2.is<V>()) {
            return sol::make_object(state, p1.as<M>() * p2.as<V>()); // vector
        } else if (p2.is<S>()) {
            return sol::make_object(state, p1.as<M>() * p2.as<S>()); // matrix
        }
    } else if (p1.is<S>()) { // if p1 is scalar, then p2 can be M or V
        if (p2.is<M>()) {
            return sol::make_object(state, p1.as<S>() * p2.as<M>()); // matrix
        } else if (p2.is<V>()) {
            return sol::make_object(state, p1.as<S>() * p2.as<V>()); // vector
        } else if (p2.is<Q>()) {
            return sol::make_object(state, p1.as<S>() * p2.as<Q>()); // quaternion
        }
    } else if (p1.is<Q>()) {
        if (p2.is<V>()) {
            if constexpr(L > 2) {
                return sol::make_object(state, p1.as<Q>() * p2.as<V>()); // vector
            }
        } else if (p2.is<Q>()) {
            return sol::make_object(state, p1.as<Q>() * p2.as<Q>()); // quaternion
        } else if (p2.is<S>()) {
            return sol::make_object(state, p1.as<Q>() * p2.as<S>()); // quaternion
        }
    }

    return sol::make_object(state, sol::nil);
}

template<typename T>
constexpr bool is_glm_vec() {
    return std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>;
}

template<typename T>
constexpr bool is_glm_mat() {
    return std::is_same_v<T, glm::mat2> || std::is_same_v<T, glm::mat3> || std::is_same_v<T, glm::mat4>;
}

template<typename T, typename C>
void registerGlmType(std::string_view name, sol::state &lua, C constructors) {
    auto newIndex = [](T &self, int i, const sol::object &val) -> auto& {
        if constexpr(is_glm_vec<T>() || std::is_same_v<T, glm::quat>) {
            return self[i] = val.as<float>();
        } else if constexpr(is_glm_mat<T>()) {
            return self[i] = val.as<glm::vec<T::length(), float>>();
        }
    };

    auto usertype = lua.new_usertype<T>(
            name,
            sol::call_constructor, constructors,
            sol::meta_function::construct, constructors,
            sol::meta_function::addition, static_cast<T (*)(const T&, const T&)>(&glm::operator+),
            sol::meta_function::subtraction, static_cast<T (*)(const T&, const T&)>(&glm::operator-),
            sol::meta_function::multiplication, &multiply<T::length()>,
            sol::meta_function::division, [](const T &t, float scalar) { return t / scalar; },
            sol::meta_function::to_string, static_cast<std::string (*)(const T&)>(&glm_to_string),
            sol::meta_function::index, [](T &self, int i) -> auto& { return self[i]; },
            sol::meta_function::new_index, newIndex,
            sol::meta_function::equal_to, static_cast<bool (*)(const T&, const T&)>(&glm::operator==));

    if constexpr(is_glm_vec<T>()) {
        usertype["x"] = &T::x;
        usertype["y"] = &T::y;

        if constexpr(std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>) {
            usertype["z"] = &T::z;
        }

        if constexpr(std::is_same_v<T, glm::vec4>) {
            usertype["w"] = &T::w;
        }

        usertype["normalize"] = static_cast<T (*)(const T&)>(&glm::normalize);

        lua[name][sol::meta_function::length] = static_cast<float (*)(const T&)>(&glm::length);
        lua[name][sol::meta_function::unary_minus] = static_cast<T (*)(const T&)>(&glm::operator-);
    }

    if constexpr(std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::quat>) {
        usertype["cross"] = static_cast<T (*)(const T&, const T&)>(&glm::cross);
    }

    if constexpr(std::is_same_v<T, glm::quat>) {
        usertype["conjugate"] = static_cast<T (*)(const T&)>(&glm::conjugate);
        usertype["inverse"] = static_cast<T (*)(const T&)>(&glm::inverse);
        usertype["mat3_cast"] = static_cast<glm::mat3 (*)(const T&)>(&glm::mat3_cast);
        usertype["mat4_cast"] = static_cast<glm::mat4 (*)(const T&)>(&glm::mat4_cast);
        usertype["quat_cast"] = sol::overload(
            static_cast<T (*)(const glm::mat3&)>(&glm::quat_cast),
            static_cast<T (*)(const glm::mat4&)>(&glm::quat_cast)
        );
        usertype["mix"] = static_cast<T (*)(const T&, const T&, typename T::value_type)>(&glm::mix);
        usertype["normalize"] = static_cast<T (*)(const T&)>(&glm::normalize);

        lua[name][sol::meta_function::length] = static_cast<float (*)(const T&)>(&glm::length);
        lua[name][sol::meta_function::unary_minus] = static_cast<T (*)(const T&)>(&glm::operator-);
    }

    if constexpr(is_glm_mat<T>()) {
        usertype["inverse"] = static_cast<T (*)(const T&)>(&glm::inverse);
        usertype["transpose"] = static_cast<T (*)(const T&)>(&glm::transpose);
    }
}

void GLMLuaTypes::registerLuaUsertype(Lua *lua) {
    lua = lua ? lua : &Engine::getLua();
    auto &state = *lua->state();

    using namespace glm;

    registerGlmType<vec2>(
            "vec2", state,
            sol::constructors<vec2(), vec2(const vec2&), vec2(float), vec2(float, float)>());

    registerGlmType<vec3>(
            "vec3", state,
            sol::constructors<vec3(), vec3(const vec3&), vec3(const vec2&, float), vec3(float),
            vec3(float, float, float)>());

    registerGlmType<glm::vec4>(
            "vec4", state,
            sol::constructors<vec4(), vec4(const vec4&), vec4(const vec3&, float),
            vec4(const vec2&, float, float), vec4(float), vec4(float, float, float, float)>());

    registerGlmType<mat2>(
            "mat2", state,
            sol::constructors<mat2(), mat2(const mat2&), mat2(const mat3&), mat2(const mat4&), mat2(float)>());

    registerGlmType<mat3>(
            "mat3", state,
            sol::constructors<mat3(), mat3(const mat3&), mat3(const mat4&), mat3(float)>());

    registerGlmType<mat4>(
            "mat4", state,
            sol::constructors<mat4(), mat4(const mat4&), mat4(float)>());

    registerGlmType<quat>(
            "quat", state,
            sol::constructors<quat(), quat(const quat&), quat(float, const vec3&), quat(float, float, float, float),
            quat(const vec3&, const vec3&), quat(const vec3&), quat(const mat3&), quat(const mat4&)>());
}
}

#ifndef ALGINE_LUA_H
#define ALGINE_LUA_H

#include <sol/state.hpp>

#include <memory>

namespace algine {
class Lua {
public:
    template<typename... Types>
    struct TypeList {};

public:
    void init();
    bool isInitialized() const;

    const std::unique_ptr<sol::state>& state() const;

    sol::global_table createEnvironment();
    sol::global_table createEnvironment(const sol::global_table &parent);
    sol::global_table& getGlobalEnvironment() const;

    template<typename T, typename G, typename S>
    static void new_property(sol::usertype<T> &usertype,
        std::string_view propName, std::string_view getterName, std::string_view setterName,
        G &&getter, S &&setter);

    template<typename T, typename G, typename S>
    static void new_property(sol::usertype<T> &usertype, std::string_view propName, G &&getter, S &&setter);

    template<typename T, typename... Args>
    void registerUsertype(sol::global_table *env = nullptr);

    template<typename... Args>
    void registerUsertype(sol::global_table *env, TypeList<Args...>) { registerUsertype<Args...>(env); }

private:
    void initEnvironment(sol::global_table &env);

private:
    std::unique_ptr<sol::state> m_lua {nullptr};
};

template<typename T, typename G, typename S>
void Lua::new_property(sol::usertype<T> &usertype,
    std::string_view propName, std::string_view getterName, std::string_view setterName,
    G &&getter, S &&setter)
{
    usertype[propName] = sol::property(getter, setter);
    usertype[getterName] = getter;
    usertype[setterName] = setter;
}

template<typename T, typename G, typename S>
void Lua::new_property(sol::usertype<T> &usertype, std::string_view propName, G &&getter, S &&setter) {
    std::string prop = propName.data();
    prop[0] = static_cast<char>(toupper(prop[0]));
    new_property(usertype, propName, "get" + prop, "set" + prop, getter, setter);
}

template<typename T, typename... Args>
void Lua::registerUsertype(sol::global_table *env) {
    T::registerLuaUsertype(this, env);

    if constexpr(sizeof...(Args) != 0) {
        registerUsertype<Args...>(env);
    }
}
} // algine

#endif //ALGINE_LUA_H

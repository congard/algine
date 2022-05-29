#ifndef ALGINE_LUA_H
#define ALGINE_LUA_H

#include "sol/state.hpp"

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

    template<typename T, typename G, typename S>
    static void new_property(sol::usertype<T> &usertype,
        std::string_view propName, std::string_view getterName, std::string_view setterName,
        G &&getter, S &&setter);

    template<typename T, typename... Args>
    void registerUsertype();

    template<typename... Args>
    void registerUsertype(TypeList<Args...>) { registerUsertype<Args...>(); }

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

template<typename T, typename... Args>
void Lua::registerUsertype() {
    T::registerLuaUsertype(this);

    if constexpr(sizeof...(Args) != 0) {
        registerUsertype<Args...>();
    }
}
} // algine

#endif //ALGINE_LUA_H

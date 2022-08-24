#include <algine/core/math/Point.h>
#include <algine/core/lua/Lua.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template<typename T>
void registerLuaUsertype(std::string_view name, sol::table &table) {
    if (table[name].valid())
        return;

    auto ctors = sol::constructors<Point<T>(), Point<T>(T, T)>();
    auto usertype = table.new_usertype<Point<T>>(
            name,
            sol::meta_function::equal_to, &Point<T>::operator==,
            sol::meta_function::addition, &Point<T>::operator+,
            sol::meta_function::subtraction, &Point<T>::operator-,
            sol::meta_function::multiplication, &Point<T>::operator*,
            sol::meta_function::division, &Point<T>::operator/,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors);

    usertype["setPos"] = &Point<T>::setPos;

    Lua::new_property(usertype, "x", &Point<T>::getX, &Point<T>::setX);
    Lua::new_property(usertype, "y", &Point<T>::getY, &Point<T>::setY);
}

template<> void registerLuaUsertype<Point<int>>(sol::table &table, void *userdata) {
    registerLuaUsertype<int>("PointI", table);
}

template<> void registerLuaUsertype<Point<float>>(sol::table &table, void *userdata) {
    registerLuaUsertype<float>("PointF", table);
}
}
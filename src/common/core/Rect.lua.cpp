#include <algine/core/Rect.h>
#include <algine/core/lua/Lua.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template<typename T>
void registerRect(std::string_view name, sol::table &table) {
    if (table[name].valid())
        return;

    auto ctors = sol::constructors<Rect<T>(), Rect<T>(T, T, T, T), Rect<T>(const Point<T>&, const Point<T>&)>();
    auto usertype = table.new_usertype<Rect<T>>(
            name,
            sol::meta_function::equal_to, &Rect<T>::operator==,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors);

    usertype["setPos"] = &Rect<T>::setPos;
    usertype["setCoords"] = &Rect<T>::setCoords;

    Lua::new_property(usertype, "x", &Rect<T>::getX, &Rect<T>::setX);
    Lua::new_property(usertype, "y", &Rect<T>::getY, &Rect<T>::setY);
    Lua::new_property(usertype, "width", &Rect<T>::getWidth, &Rect<T>::setWidth);
    Lua::new_property(usertype, "height", &Rect<T>::getHeight, &Rect<T>::setHeight);
}

template<> void registerLuaUsertype<Rect<int>>(sol::table &table, void *userdata) {
    registerRect<int>("RectI", table);
}

template<> void registerLuaUsertype<Rect<float>>(sol::table &table, void *userdata) {
    registerRect<float>("RectF", table);
}
}

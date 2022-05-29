#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/core/Engine.h>

#include <tulz/Array.h>

namespace algine {
template<typename T>
void registerArray(std::string_view name, sol::state &lua) {
    using array = tulz::Array<T>;

    auto to_string = [](const array &self) -> std::string {
        if (self.empty())
            return "[]";

        std::string result = "[";

        for (int i = 0; i < self.size(); ++i)
            result += std::to_string(self[i]) + ", ";

        result.erase(result.length() - 1);
        result[result.length() - 1] = ']';

        return result;
    };

    auto ctors = sol::constructors<array(), array(size_t), array(size_t, const T&), array(const array&)>();
    auto usertype = lua.new_usertype<array>(
            name,
            sol::call_constructor, ctors,
            sol::meta_function::construct, ctors,
            sol::meta_function::index, static_cast<const T& (array::*)(size_t) const>(&array::operator[]),
            sol::meta_function::new_index, [](array &self, size_t index, T value) { self[index] = value; },
            sol::meta_function::length, &array::size,
            sol::meta_function::to_string, to_string);

    usertype["empty"] = &array::empty;
    usertype["resize"] = sol::overload(
        static_cast<void (array::*)(size_t)>(&array::resize),
        static_cast<void (array::*)(size_t, const T&)>(&array::resize)
    );
}

void TulzLuaTypes::registerLuaUsertype(Lua *lua) {
    lua = lua ? lua : &Engine::getLua();
    auto &state = *lua->state();

    registerArray<uint>("UnsignedArray", state);
    registerArray<int>("IntArray", state);
    registerArray<float>("FloatArray", state);
    registerArray<double>("DoubleArray", state);
}
} // algine
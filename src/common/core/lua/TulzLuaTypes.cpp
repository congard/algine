#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/core/Engine.h>
#include <algine/core/lua/Scriptable.h>

#include <tulz/Array.h>

namespace algine {
template<typename T>
void registerArray(std::string_view name, sol::global_table &env) {
    using array = tulz::Array<T>;

    auto to_string = [](const array &self, sol::this_environment tenv) -> std::string {
        if (self.empty())
            return "[]";

        auto env = *tenv.env;
        std::string result = "[";

        if constexpr(std::is_same_v<T, sol::object>) {
            sol::function tostring = env["tostring"];

            for (int i = 0; i < self.size(); ++i) {
                const sol::object &obj = self[i];
                result += tostring(obj).get<std::string>() + ", ";
            }
        } else {
            for (int i = 0; i < self.size(); ++i) {
                result += std::to_string(self[i]) + ", ";
            }
        }

        result.erase(result.length() - 1);
        result[result.length() - 1] = ']';

        return result;
    };

    auto ctors = sol::constructors<array(), array(size_t), array(size_t, const T&), array(const array&)>();
    auto usertype = env.new_usertype<array>(
            name,
            sol::call_constructor, ctors,
            sol::meta_function::construct, ctors,
            sol::meta_function::index, static_cast<const T& (array::*)(size_t) const>(&array::operator[]),
            sol::meta_function::new_index, [](array &self, size_t index, T value) { self[index] = std::move(value); },
            sol::meta_function::length, &array::size,
            sol::meta_function::to_string, to_string);

    usertype["empty"] = &array::empty;
    usertype["resize"] = sol::overload(
        static_cast<void (array::*)(size_t)>(&array::resize),
        static_cast<void (array::*)(size_t, const T&)>(&array::resize)
    );
}

void TulzLuaTypes::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = Scriptable::getEnv(lua, tenv);

    registerArray<sol::object>("Array", env);
    registerArray<uint>("UnsignedArray", env);
    registerArray<int>("IntArray", env);
    registerArray<float>("FloatArray", env);
    registerArray<double>("DoubleArray", env);
}
} // algine
#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/core/Engine.h>

#include <tulz/container/Array.h>
#include <tulz/Path.h>

using namespace tulz;

namespace algine {
template<typename T>
void registerArray(std::string_view name, sol::environment &env) {
    if (Lua::isRegistered(env, name))
        return;

    using array = tulz::Array<T>;

    auto to_string = [env](const array &self) -> std::string {
        if (self.empty())
            return "[]";

        std::string result = "[";

        if constexpr(std::is_same_v<T, sol::object>) {
            sol::function tostring = sol::environment(env)["tostring"];

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

void registerPath(sol::environment &env) {
    if (Lua::isRegistered(env, "Path"))
        return;

    auto ctors = sol::constructors<Path(), Path(const std::string&)>();
    auto usertype = env.new_usertype<Path>(
            "Path",
            sol::meta_function::to_string, &Path::toString,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors);

    usertype["setPath"] = &Path::setPath;
    usertype["exists"] = &Path::exists;
    usertype["isFile"] = &Path::isFile;
    usertype["isDirectory"] = &Path::isDirectory;
    usertype["isAbsolute"] = &Path::isAbsolute;
    usertype["size"] = &Path::size;
    usertype["toString"] = &Path::toString;
    usertype["getPathName"] = &Path::getPathName;
    usertype["getParentDirectory"] = &Path::getParentDirectory;
    usertype["listChildren"] = &Path::listChildren;

    // static
    usertype["setWorkingDirectory"] = sol::overload(
        static_cast<void (*)(const Path&)>(&Path::setWorkingDirectory),
        static_cast<void (*)(const std::string&)>(&Path::setWorkingDirectory));
    usertype["getWorkingDirectory"] = &Path::getWorkingDirectory;
    usertype["getSystemPath"] = &Path::getSystemPath;
    usertype["join"] = sol::overload(
        static_cast<Path (*)(const Path&, const Path&)>(&Path::join),
        static_cast<std::string (*)(const std::string&, const std::string&)>(&Path::join));
}

void TulzLuaTypes::registerLuaUsertype(Lua *lua, sol::environment *tenv) {
    auto env = Lua::getEnv(lua, tenv);

    registerArray<sol::object>("Array", env);
    registerArray<ubyte>("UnsignedByteArray", env);
    registerArray<uint>("UnsignedArray", env);
    registerArray<int>("IntArray", env);
    registerArray<float>("FloatArray", env);
    registerArray<double>("DoubleArray", env);

    registerPath(env);
}
} // algine
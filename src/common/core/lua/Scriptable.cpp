#include <algine/core/lua/Scriptable.h>

namespace algine {
void Scriptable::execute(const std::string &path, Lua *lua) {
    exec(path, true, lua);
}

void Scriptable::executeString(const std::string &str, Lua *lua) {
    exec(str, false, lua);
}

void Scriptable::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Scriptable"))
        return;

    auto usertype = lua->state()->new_usertype<Scriptable>(
            "Scriptable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor);

    usertype["execute"] = [&](const sol::object &self, const std::string &path) {
        self.as<Scriptable>().execute(path, lua);
    };

    usertype["executeString"] = [&](const sol::object &self, const std::string &str) {
        self.as<Scriptable>().executeString(str, lua);
    };
}

void Scriptable::exec(const std::string &s, bool path, Lua *lua) {
    throw std::runtime_error("Stub! Not implemented!");
}

Lua* Scriptable::getLua(Lua *lua) {
    if (lua == nullptr)
        return &Engine::getLua();

    if (!lua->isInitialized())
        lua->init();

    return lua;
}

bool Scriptable::isRegistered(Lua &lua, std::string_view type) {
    return (*lua.state())[type].valid();
}
} // algine
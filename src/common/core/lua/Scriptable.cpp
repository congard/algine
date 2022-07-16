#include <algine/core/lua/Scriptable.h>

namespace algine {
void Scriptable::execute(const std::string &path, Lua *lua, sol::global_table *env) {
    exec(path, true, lua, env);
}

void Scriptable::executeString(const std::string &str, Lua *lua, sol::global_table *env) {
    exec(str, false, lua, env);
}

void Scriptable::setRootDir(std::string_view rootDir) {
    m_rootDir = rootDir;
}

const std::string& Scriptable::getRootDir() const {
    return m_rootDir;
}

void Scriptable::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "Scriptable"))
        return;

    auto usertype = env.new_usertype<Scriptable>(
            "Scriptable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor);

    usertype["execute"] = [lua, tenv](const sol::object &self, const std::string &path) {
        self.as<Scriptable>().execute(path, lua, tenv);
    };

    usertype["executeString"] = [lua, tenv](const sol::object &self, const std::string &str) {
        self.as<Scriptable>().executeString(str, lua, tenv);
    };

    Lua::new_property(usertype, "rootDir", &Scriptable::getRootDir, &Scriptable::setRootDir);
}

void Scriptable::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    throw std::runtime_error("Stub! Not implemented!");
}

sol::global_table& Scriptable::getEnv(Lua *lua, sol::global_table *env) {
    if (env)
        return *env;

    if (!lua)
        return Engine::getLua().state()->globals();

    if (!lua->isInitialized())
        lua->init();

    return lua->state()->globals();
}

bool Scriptable::isRegistered(sol::global_table &env, std::string_view type) {
    return env[type].valid();
}
} // algine
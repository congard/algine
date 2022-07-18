#include <algine/core/lua/Scriptable.h>

#include <tulz/Path.h>

using namespace tulz;

namespace algine {
void Scriptable::execute(const std::string &in_path, Lua *lua, sol::global_table *env) {
    std::string path = in_path;

    if (m_rootDir.empty()) {
        Path p(path);
        m_rootDir = p.getParentDirectory().toString();
        path = p.getPathName();
    }

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

    IOProvider::registerLuaUsertype(lua, tenv);

    auto usertype = env.new_usertype<Scriptable>(
            "Scriptable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<IOProvider>());

    usertype["execute"] = [lua, tenv](const sol::object &self, const std::string &path) {
        self.as<Scriptable>().execute(path, lua, tenv);
    };

    usertype["executeString"] = [lua, tenv](const sol::object &self, const std::string &str) {
        self.as<Scriptable>().executeString(str, lua, tenv);
    };

    Lua::new_property(usertype, "rootDir", &Scriptable::getRootDir, &Scriptable::setRootDir);
}
} // algine
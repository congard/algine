#ifndef ALGINE_SCRIPTABLE_H
#define ALGINE_SCRIPTABLE_H

#include <algine/core/lua/Lua.h>
#include <algine/core/io/IOProvider.h>

#include <tulz/Path.h>

namespace algine {
class AL_EXPORT Scriptable: public IOProvider {
public:
    void execute(const std::string &path, Lua *lua = nullptr, sol::environment *env = nullptr);
    void executeString(const std::string &str, Lua *lua = nullptr, sol::environment *env = nullptr);

    void setRootDir(std::string_view rootDir);
    const std::string& getRootDir() const;

protected:
    Scriptable() = default;

protected:
    static inline auto getEnv(Lua *lua, sol::environment *tenv) { return Lua::getEnv(lua, tenv); }
    static inline bool isRegistered(sol::environment &env, std::string_view type) { return Lua::isRegistered(env, type); }

    virtual void exec(const std::string &s, bool path, Lua *lua, sol::environment *env) = 0;

    template<typename T>
    void exec_t(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
        lua = lua ? lua : &Lua::getDefault();
        Lua::Locker locker(lua);
        auto env = getEnv(lua, tenv);
        Lua::registerUsertype<T>(lua, tenv);

        auto &state = lua->state();

        auto printErr = [&](sol::error &error) {
            printExecErr(error, s, path);
        };

        try {
            sol::environment luaEnv(env);

            if (path) {
                state.script(readStr(tulz::Path::join(m_rootDir, s)), luaEnv);
            } else {
                state.script(s, luaEnv);
            }

            auto result = ((sol::function) luaEnv["main"])(dynamic_cast<T*>(this));

            // in case if exceptions are disabled
            if (!result.valid()) {
                sol::error error = result;
                printErr(error);
            }
        } catch (sol::error &error) {
            printErr(error);
        }
    }

private:
    void printExecErr(sol::error &error, const std::string &s, bool path);

private:
    std::string m_rootDir;
};
} // algine

#endif //ALGINE_SCRIPTABLE_H

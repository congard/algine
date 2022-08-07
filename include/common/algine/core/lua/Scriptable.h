#ifndef ALGINE_SCRIPTABLE_H
#define ALGINE_SCRIPTABLE_H

#include <algine/core/Engine.h>
#include <algine/core/io/IOProvider.h>
#include <algine/core/log/Log.h>

#include <tulz/Path.h>

namespace algine {
class Scriptable: public IOProvider {
public:
    void execute(const std::string &path, Lua *lua = nullptr, sol::global_table *env = nullptr);
    void executeString(const std::string &str, Lua *lua = nullptr, sol::global_table *env = nullptr);

    void setRootDir(std::string_view rootDir);
    const std::string& getRootDir() const;

protected:
    Scriptable() = default;

protected:
    static inline auto& getEnv(Lua *lua, sol::global_table *tenv) { return Lua::getEnv(lua, tenv); }
    static inline bool isRegistered(sol::global_table &env, std::string_view type) { return Lua::isRegistered(env, type); }

    virtual void exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) = 0;

    template<typename T>
    void exec_t(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
        lua = lua ? lua : &Engine::getLua();
        Lua::Locker locker(lua);
        auto &env = getEnv(lua, tenv);
        T::registerLuaUsertype(lua, tenv);

        auto &state = *lua->state();

        auto printErr = [&](sol::error &error) {
            if (!path) {
                Log::error() << error.what();
            } else {
                Log::error() << "In " << tulz::Path::join(m_rootDir, s) << ":\n" << error.what();
            }
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
    std::string m_rootDir;
};
} // algine

#endif //ALGINE_SCRIPTABLE_H

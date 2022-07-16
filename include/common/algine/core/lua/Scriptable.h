#ifndef ALGINE_SCRIPTABLE_H
#define ALGINE_SCRIPTABLE_H

#include <algine/core/Engine.h>

namespace algine {
class Scriptable {
public:
    void execute(const std::string &path, Lua *lua = nullptr, sol::global_table *env = nullptr);
    void executeString(const std::string &str, Lua *lua = nullptr, sol::global_table *env = nullptr);

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv = nullptr); // must be implemented in derived classes

    static sol::global_table& getEnv(Lua *lua, sol::global_table *tenv = nullptr);
    static bool isRegistered(sol::global_table &env, std::string_view type);

protected:
    virtual void exec(const std::string &s, bool path, Lua *lua, sol::global_table *env);

    template<typename T>
    void exec_t(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
        auto &env = getEnv(lua, tenv);
        T::registerLuaUsertype(lua, tenv);

        auto &state = lua ? *lua->state() : *Engine::getLua().state();

        try {
            sol::environment luaEnv(env);

            if (path) {
                state.script_file(s, luaEnv);
            } else {
                state.script(s, luaEnv);
            }

            auto result = ((sol::function) luaEnv["main"])(dynamic_cast<T*>(this));

            // in case if exceptions are disabled
            if (!result.valid()) {
                sol::error error = result;
                fprintf(stderr, "%s\n", error.what());
            }
        } catch (sol::error &error) {
            fprintf(stderr, "%s\n", error.what());
        }
    }
};
} // algine

#endif //ALGINE_SCRIPTABLE_H

#ifndef ALGINE_SCRIPTABLE_H
#define ALGINE_SCRIPTABLE_H

#include "algine/core/Engine.h"

namespace algine {
class Scriptable {
public:
    void execute(const std::string &path, Lua *lua = nullptr);
    void executeString(const std::string &str, Lua *lua = nullptr);

    static void registerLuaUsertype(Lua *lua); // must be implemented in derived classes

protected:
    virtual void exec(const std::string &s, bool path, Lua *lua);

    static Lua* getLua(Lua *lua);
    static bool isRegistered(Lua &lua, std::string_view type);

    template<typename T>
    void exec_t(const std::string &s, bool path, Lua *lua) {
        lua = getLua(lua);
        T::registerLuaUsertype(lua);

        auto &state = *lua->state();

        try {
            if (path) {
                state.script_file(s);
            } else {
                state.script(s);
            }

            auto result = ((sol::function) state["main"])(dynamic_cast<T*>(this));

            // in case if exceptions are disabled
            if (!result.valid()) {
                sol::error error = result;
                std::cout << error.what() << std::endl;
            }
        } catch (...) {
            // TODO
        }
    }
};
} // algine

#endif //ALGINE_SCRIPTABLE_H

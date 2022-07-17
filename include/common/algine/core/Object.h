#ifndef ALGINE_OBJECT_H
#define ALGINE_OBJECT_H

#include <algine/core/lua/Scriptable.h>

#include <string>

namespace algine {
class Object {
public:
    void setName(const std::string &name);
    const std::string& getName() const;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    static inline auto& getEnv(Lua *lua, sol::global_table *tenv) { return Lua::getEnv(lua, tenv); }
    static inline bool isRegistered(sol::global_table &env, std::string_view type) { return Lua::isRegistered(env, type); }

private:
    std::string m_name;
};
}

#endif //ALGINE_OBJECT_H

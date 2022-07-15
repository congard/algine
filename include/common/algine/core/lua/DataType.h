#ifndef ALGINE_LUA_DATATYPE_H
#define ALGINE_LUA_DATATYPE_H

#include <algine/core/lua/Lua.h>

namespace algine::lua {
class DataType {
public:
    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);
};
}

#endif //ALGINE_LUA_DATATYPE_H

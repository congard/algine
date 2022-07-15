#ifndef ALGINE_GLMLUATYPES_H
#define ALGINE_GLMLUATYPES_H

#include <algine/core/lua/Lua.h>

namespace algine {
class GLMLuaTypes {
public:
    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);
};
}

#endif //ALGINE_GLMLUATYPES_H

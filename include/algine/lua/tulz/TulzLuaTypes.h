#ifndef ALGINE_TULZLUATYPES_H
#define ALGINE_TULZLUATYPES_H

#include <algine/lua/Lua.h>

namespace algine {
class TulzLuaTypes {
public:
    static void registerLuaUsertype(Lua *lua, sol::environment *tenv);
};
} // algine

#endif //ALGINE_TULZLUATYPES_H

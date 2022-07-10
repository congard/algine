#ifndef ALGINE_LUA_DATATYPE_H
#define ALGINE_LUA_DATATYPE_H

namespace algine {
class Lua;

namespace lua {
class DataType {
public:
    static void registerLuaUsertype(Lua *lua);
};
}
}

#endif //ALGINE_LUA_DATATYPE_H

#include <algine/core/lua/DataType.h>
#include <algine/core/lua/Scriptable.h>
#include <algine/core/DataType.h>

using EDataType = algine::DataType;

namespace algine::lua {
void DataType::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = algine::Scriptable::getEnv(lua, tenv);

    if (algine::Scriptable::isRegistered(env, "DataType"))
        return;

    env.new_enum("DataType",
        "Byte", EDataType::Byte,
        "UnsignedByte", EDataType::UnsignedByte,
        "Short", EDataType::Short,
        "UnsignedShort", EDataType::UnsignedShort,
        "Int", EDataType::Int,
        "UnsignedInt", EDataType::UnsignedInt,
        "Float", EDataType::Float,
        "HalfFloat", EDataType::HalfFloat,
        "Double", EDataType::Double
    );
}
}

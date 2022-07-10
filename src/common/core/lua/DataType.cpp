#include <algine/core/lua/DataType.h>
#include <algine/core/lua/Lua.h>
#include <algine/core/Engine.h>
#include <algine/core/DataType.h>

using EDataType = algine::DataType;

namespace algine::lua {
void DataType::registerLuaUsertype(Lua *lua) {
    lua = lua ? lua : &Engine::getLua();

    if ((*lua->state())["DataType"].valid())
        return;

    lua->state()->new_enum("DataType",
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

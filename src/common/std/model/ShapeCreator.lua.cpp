#include <algine/std/model/ShapeCreator.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template<typename T>
void registerBufferData(sol::table &table, std::string_view name) {
    sol::table usertypeTable = table["ShapeCreator"].get<sol::table>();

    if (usertypeTable[name].valid())
        return;

    auto ctors = sol::constructors<ShapeCreator::BufferData<T>()>();
    auto usertype = usertypeTable.new_usertype<ShapeCreator::BufferData<T>>(
            name,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors);
    usertype["data"] = &ShapeCreator::BufferData<T>::data;
    usertype["usage"] = &ShapeCreator::BufferData<T>::usage;
}

template<> void registerLuaUsertype<ShapeCreator::BufferData<float>>(sol::table &table, void *userdata) {
    registerBufferData<float>(table, "FloatBufferData");
}

template<> void registerLuaUsertype<ShapeCreator::BufferData<uint>>(sol::table &table, void *userdata) {
    registerBufferData<uint>(table, "UnsignedBufferData");
}
}

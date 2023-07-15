#include <algine/std/model/ShapeBuilder.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template<typename T>
void registerBufferData(sol::table &table, std::string_view name) {
    sol::table usertypeTable = table["ShapeBuilder"].get<sol::table>();

    if (usertypeTable[name].valid())
        return;

    auto ctors = sol::constructors<ShapeBuilder::BufferData<T>()>();
    auto usertype = usertypeTable.new_usertype<ShapeBuilder::BufferData<T>>(
            name,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors);
    usertype["data"] = &ShapeBuilder::BufferData<T>::data;
    usertype["usage"] = &ShapeBuilder::BufferData<T>::usage;
}

template<> void registerLuaUsertype<ShapeBuilder::BufferData<float>>(sol::table &table, void *userdata) {
    registerBufferData<float>(table, "FloatBufferData");
}

template<> void registerLuaUsertype<ShapeBuilder::BufferData<uint>>(sol::table &table, void *userdata) {
    registerBufferData<uint>(table, "UnsignedBufferData");
}
}

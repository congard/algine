#include <algine/core/FramebufferCreator.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template <typename T>
void registerAttachments(sol::table &table, std::string_view name) {
    sol::table usertypeTable = table["FramebufferCreator"].get<sol::table>();

    if (usertypeTable[name].valid())
        return;
    
    auto ctors = sol::constructors<T()>();
    auto usertype = usertypeTable.new_usertype<T>(
        name,
        sol::meta_function::construct, ctors,
        sol::call_constructor, ctors);

    usertype["addCreator"] = &T::addCreator;
    usertype["addName"] = &T::addName;
}

template<> void registerLuaUsertype<FramebufferCreator::RenderbufferAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferCreator::RenderbufferAttachments>(table, "RenderbufferAttachments");
}

template<> void registerLuaUsertype<FramebufferCreator::Texture2DAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferCreator::Texture2DAttachments>(table, "Texture2DAttachments");
}

template<> void registerLuaUsertype<FramebufferCreator::TextureCubeAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferCreator::TextureCubeAttachments>(table, "TextureCubeAttachments");
}
}

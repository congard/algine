#include <algine/core/FramebufferBuilder.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template <typename T>
void registerAttachments(sol::table &table, std::string_view name) {
    sol::table usertypeTable = table["FramebufferBuilder"].get<sol::table>();

    if (usertypeTable[name].valid())
        return;
    
    auto ctors = sol::constructors<T()>();
    auto usertype = usertypeTable.new_usertype<T>(
        name,
        sol::meta_function::construct, ctors,
        sol::call_constructor, ctors);

    usertype["addBuilder"] = &T::addBuilder;
    usertype["addName"] = &T::addName;
}

template<> void registerLuaUsertype<FramebufferBuilder::RenderbufferAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferBuilder::RenderbufferAttachments>(table, "RenderbufferAttachments");
}

template<> void registerLuaUsertype<FramebufferBuilder::Texture2DAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferBuilder::Texture2DAttachments>(table, "Texture2DAttachments");
}

template<> void registerLuaUsertype<FramebufferBuilder::TextureCubeAttachments>(sol::table &table, void *userdata) {
    registerAttachments<FramebufferBuilder::TextureCubeAttachments>(table, "TextureCubeAttachments");
}
}

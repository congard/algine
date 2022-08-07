#include <algine/core/shader/ShaderDefinitionGenerator.h>

using namespace algine;

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);

template<> void registerLuaUsertype<ShaderDefinitionGenerator::Definition>(sol::table &table, void *userdata) {
    auto parentTable = table["ShaderDefinitionGenerator"].get<sol::table>();

    if (parentTable["Definition"].valid())
        return;

    auto ctors = sol::constructors<ShaderDefinitionGenerator::Definition(),
        ShaderDefinitionGenerator::Definition(const std::string&, const std::string&)>();
    auto usertype = parentTable.new_usertype<ShaderDefinitionGenerator::Definition>(
        "Definition",
        sol::meta_function::construct, ctors,
        sol::call_constructor, ctors);
    usertype["key"] = &ShaderDefinitionGenerator::Definition::first;
    usertype["value"] = &ShaderDefinitionGenerator::Definition::second;
}
}

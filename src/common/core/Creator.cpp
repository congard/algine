#include <algine/core/Creator.h>

using namespace std;

namespace algine {
Creator::Creator()
    : m_access(Access::Private) {}

void Creator::setName(const string &name) {
    m_name = name;
}

void Creator::setAccess(Access access) {
    m_access = access;
}

const string& Creator::getName() const {
    return m_name;
}

Creator::Access Creator::getAccess() const {
    return m_access;
}

void Creator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "Creator"))
        return;

    lua->registerUsertype<Scriptable>(tenv);

    auto usertype = env.new_usertype<Creator>(
            "Creator", sol::base_classes, sol::bases<Scriptable, IOProvider>());

    Lua::new_property(usertype, "name", &Creator::getName, &Creator::setName);
    Lua::new_property(usertype, "access", &Creator::getAccess, &Creator::setAccess);

    env["Creator"].get<sol::table>().new_enum(
        "Access",
        "Private", Access::Private,
        "Public", Access::Public);
}
}

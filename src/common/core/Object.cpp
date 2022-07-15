#include <algine/core/Object.h>

using namespace std;

namespace algine {
void Object::setName(const string &name) {
    m_name = name;
}

const string& Object::getName() const {
    return m_name;
}

void Object::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "Object"))
        return;

    // note: without constructors, it is assumed that this
    // class will not be created in Lua, implemented in
    // inheritance purposes

    auto usertype = env.new_usertype<Object>("Object", sol::base_classes, sol::bases<Scriptable>());
    Lua::new_property(usertype, "name", &Object::getName, &Object::setName);
}
}

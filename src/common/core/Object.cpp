#include <algine/core/Object.h>

using namespace std;

namespace algine {
void Object::setName(const string &name) {
    m_name = name;
}

const string& Object::getName() const {
    return m_name;
}

void Object::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Object"))
        return;

    // note: without constructors, it is assumed that this
    // class will not be created in Lua, implemented in
    // inheritance purposes

    auto usertype = lua->state()->new_usertype<Object>(
            "Object",
            sol::base_classes, sol::bases<Scriptable>());
    Lua::new_property(usertype, "name", &Object::getName, &Object::setName);
}
}

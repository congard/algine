#include <algine/core/Object.h>

#include <algine/core/JsonHelper.h>

using namespace nlohmann;
using namespace std;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Name, "name");
}

namespace algine {
void Object::setName(const string &name) {
    m_name = name;
}

const string& Object::getName() const {
    return m_name;
}

void Object::import(const JsonHelper &jsonHelper) {
    m_name = jsonHelper.readValue<string>(Config::Name);
}

JsonHelper Object::dump() {
    json config;
    config[Config::Name] = m_name;
    return config;
}

void Object::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "Object"))
        return;

    // note: without constructors, it is assumed that this
    // class will not be created in Lua, implemented in
    // inheritance purposes

    auto usertype = lua->state()->new_usertype<Object>("Object");
    Lua::new_property(usertype, "name", "getName", "setName", &Object::getName, &Object::setName);
}
}

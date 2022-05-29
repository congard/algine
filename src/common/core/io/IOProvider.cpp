#include <algine/core/io/IOProvider.h>
#include <algine/core/Engine.h>

namespace algine {
IOProvider::IOProvider()
    : m_ioSystem(nullptr) {}

void IOProvider::setIOSystem(IOSystem *ioSystem) {
    m_ioSystem.reset(ioSystem);
}

void IOProvider::setIOSystem(const std::shared_ptr<IOSystem> &ioSystem) {
    m_ioSystem = ioSystem;
}

const std::shared_ptr<IOSystem>& IOProvider::getIOSystem() const {
    return io();
}

void IOProvider::writeStr(const std::string &path, const std::string &str) const {
    auto stream = io()->open(path, IOStream::Mode::WriteText);
    stream->writeStr(str);
    stream->close();
}

std::string IOProvider::readStr(const std::string &path) const {
    auto stream = io()->open(path, IOStream::Mode::ReadText);
    std::string str = stream->readStr();
    stream->close();
    return str;
}

void IOProvider::registerLuaUsertype(Lua *lua) {
    lua = getLua(lua);

    if (isRegistered(*lua, "IOProvider"))
        return;

    Scriptable::registerLuaUsertype(lua);

    auto usertype = lua->state()->new_usertype<IOProvider>("IOProvider", sol::base_classes, sol::bases<Scriptable>());
    usertype["setIOSystem_raw"] = static_cast<void (IOProvider::*)(IOSystem*)>(&IOProvider::setIOSystem);
    usertype["writeStr"] = &IOProvider::writeStr;
    usertype["readStr"] = &IOProvider::readStr;

    Lua::new_property(usertype, "ioSystem", "getIOSystem", "setIOSystem",
        &IOProvider::getIOSystem, static_cast<void (IOProvider::*)(const std::shared_ptr<IOSystem>&)>(&IOProvider::setIOSystem));
}

void IOProvider::exec(const std::string &s, bool path, Lua *lua) {
    exec_t<IOProvider>(s, path, lua);
}

const std::shared_ptr<IOSystem>& IOProvider::io() const {
    if (m_ioSystem == nullptr) {
        return Engine::getDefaultIOSystem();
    } else {
        return m_ioSystem;
    }
}
}

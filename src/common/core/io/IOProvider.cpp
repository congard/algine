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

bool IOProvider::exists(const std::string &path) const {
    return io()->exists(path);
}

void IOProvider::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = Lua::getEnv(lua, tenv);

    if (Lua::isRegistered(env, "IOProvider"))
        return;

    auto usertype = env.new_usertype<IOProvider>(
            "IOProvider",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor);
    usertype["setIOSystem_raw"] = static_cast<void (IOProvider::*)(IOSystem*)>(&IOProvider::setIOSystem);
    usertype["writeStr"] = &IOProvider::writeStr;
    usertype["readStr"] = &IOProvider::readStr;
    usertype["exists"] = &IOProvider::exists;

    Lua::new_property(usertype, "ioSystem", "getIOSystem", "setIOSystem",
        &IOProvider::getIOSystem, static_cast<void (IOProvider::*)(const std::shared_ptr<IOSystem>&)>(&IOProvider::setIOSystem));
}

const std::shared_ptr<IOSystem>& IOProvider::io() const {
    if (m_ioSystem == nullptr) {
        return Engine::getDefaultIOSystem();
    } else {
        return m_ioSystem;
    }
}
}

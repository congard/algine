#include <algine/core/lua/Scriptable.h>

#include <tulz/Path.h>

using namespace tulz;

namespace algine {
void Scriptable::execute(const std::string &in_path, Lua *lua, sol::global_table *env) {
    std::string path = in_path;

    if (m_rootDir.empty()) {
        Path p(path);
        m_rootDir = p.getParentDirectory().toString();
        path = p.getPathName();
    }

    exec(path, true, lua, env);
}

void Scriptable::executeString(const std::string &str, Lua *lua, sol::global_table *env) {
    exec(str, false, lua, env);
}

void Scriptable::setRootDir(std::string_view rootDir) {
    m_rootDir = rootDir;
}

const std::string& Scriptable::getRootDir() const {
    return m_rootDir;
}
} // algine
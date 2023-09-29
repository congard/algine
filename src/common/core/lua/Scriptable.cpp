#include <algine/core/lua/Scriptable.h>
#include <algine/core/log/Log.h>

#include <tulz/Path.h>

using namespace tulz;

namespace algine {
void Scriptable::execute(const std::string &in_path, Lua *lua, sol::environment *env) {
    std::string path = in_path;

    if (m_rootDir.empty()) {
        Path p(path);
        m_rootDir = p.getParentDirectory().toString();
        path = p.getPathName();
    }

    exec(path, true, lua, env);
}

void Scriptable::executeString(const std::string &str, Lua *lua, sol::environment *env) {
    exec(str, false, lua, env);
}

void Scriptable::setRootDir(std::string_view rootDir) {
    m_rootDir = rootDir;
}

const std::string& Scriptable::getRootDir() const {
    return m_rootDir;
}

void Scriptable::printExecErr(sol::error &error, const std::string &s, bool path) {
    if (!path) {
        Log::error() << error.what();
    } else {
        Log::error() << "In " << tulz::Path::join(m_rootDir, s) << ":\n" << error.what();
    }
}
} // algine
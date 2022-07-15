#include <algine/core/transfer/FileTransferable.h>

#include <algine/core/JsonHelper.h>

#include <tulz/File.h>

using namespace std;
using namespace tulz;

namespace algine {
void FileTransferable::setWorkingDirectory(const string &path) {
    m_workingDirectory = path;
}

const string& FileTransferable::getWorkingDirectory() const {
    return m_workingDirectory;
}

void FileTransferable::importFromFile(const string &path) {
    string m_confPath;

    if (!m_workingDirectory.empty()) {
        m_confPath = Path::join(m_workingDirectory, path);
        m_workingDirectory = Path(m_confPath).getParentDirectory().toString();
    } else {
        m_confPath = path;
        m_workingDirectory = Path(path).getParentDirectory().toString();
    }

    import(readStr(m_confPath));
}

void FileTransferable::dumpToFile(const string &path) {
    writeStr(path, dump().toString());
}

void FileTransferable::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "FileTransferable"))
        return;

    IOProvider::registerLuaUsertype(lua, tenv);

    auto usertype = env.new_usertype<FileTransferable>(
            "FileTransferable",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<IOProvider>());

    Lua::new_property(usertype, "workingDirectory", &FileTransferable::getWorkingDirectory, &FileTransferable::setWorkingDirectory);
}
}

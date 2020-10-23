#include <algine/core/ManagerBase.h>

#include <algine/core/JsonHelper.h>

#include <algine/types.h>

#include <tulz/Path.h>

using namespace std;
using namespace nlohmann;
using namespace tulz;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Name, "name");
constant(Access, "access");

constant(Private, "private");
constant(Public, "public");
}

namespace algine {
ManagerBase::ManagerBase()
    : m_access(Access::Private)
{
    // see initializer list above
}

void ManagerBase::setName(const string &name) {
    m_name = name;
}

void ManagerBase::setAccess(Access access) {
    m_access = access;
}

const string& ManagerBase::getName() const {
    return m_name;
}

ManagerBase::Access ManagerBase::getAccess() const {
    return m_access;
}

void ManagerBase::setWorkingDirectory(const string &path) {
    m_workingDirectory = path;
}

const string& ManagerBase::getWorkingDirectory() const {
    return m_workingDirectory;
}

void ManagerBase::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // load name
    if (config.contains(Name))
        m_name = config[Name];

    // load access
    if (config.contains(Config::Access)) {
        m_access = map<string, Access> {
            {Private, Access::Private},
            {Public, Access::Public}
        } [config[Config::Access]];
    }
}

JsonHelper ManagerBase::dump() {
    using namespace Config;

    json config;

    // write name
    if (!m_name.empty())
        config[Name] = m_name;

    // write access
    config[Config::Access] = vector<string> {Private, Public} [static_cast<uint>(m_access)];

    return config;
}

void ManagerBase::importFromFile(const string &path) {
    if (!m_workingDirectory.empty()) {
        m_confPath = Path::join(m_workingDirectory, path);
        m_workingDirectory = Path(m_confPath).getParentDirectory();
    } else {
        m_confPath = path;
        m_workingDirectory = Path(path).getParentDirectory();
    }

    Transferable::importFromFile(m_confPath);
}
}

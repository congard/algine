#include <algine/core/Creator.h>

#include <algine/core/JsonHelper.h>

#include <algine/types.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace Config {
constant(Name, "name");
constant(Access, "access");

constant(Private, "private");
constant(Public, "public");
}

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

void Creator::import(const JsonHelper &jsonHelper) {
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

JsonHelper Creator::dump() {
    using namespace Config;

    json config;

    // write name
    if (!m_name.empty())
        config[Name] = m_name;

    // write access
    config[Config::Access] = vector<string> {Private, Public} [static_cast<uint>(m_access)];

    return config;
}
}

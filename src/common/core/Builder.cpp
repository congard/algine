#include <algine/core/Builder.h>

using namespace std;

namespace algine {
Builder::Builder()
    : m_access(Access::Private) {}

void Builder::setName(const string &name) {
    m_name = name;
}

void Builder::setAccess(Access access) {
    m_access = access;
}

const string& Builder::getName() const {
    return m_name;
}

Builder::Access Builder::getAccess() const {
    return m_access;
}
}

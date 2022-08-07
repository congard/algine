#include <algine/core/Creator.h>

using namespace std;

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
}

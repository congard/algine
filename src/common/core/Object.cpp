#include <algine/core/Object.h>

using namespace std;

namespace algine {
void Object::setName(const string &name) {
    m_name = name;
}

const string& Object::getName() const {
    return m_name;
}
}

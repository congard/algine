#include <algine/core/TypeRegistry.h>

#include <stdexcept>

using namespace std;

namespace algine {
map<string, void*> TypeRegistry::m_registry;

void TypeRegistry::registerType(const string &name, void *creator) {
    m_registry[name] = creator;
}

void TypeRegistry::unregister(const string &name) {
    if (auto it = m_registry.find(name); it != m_registry.end()) {
        delete (Creator<> *) it->second;
        m_registry.erase(it);
    } else {
        throw runtime_error("Type '" + name + "' does not found");
    }
}

bool TypeRegistry::contains(const string &name) {
    return m_registry.find(name) != m_registry.end();
}

TypeRegistry::Deleter TypeRegistry::getDeleter(const string &name) {
    return ((Creator<> *) m_registry[name])->getDeleter();
}

void TypeRegistry::clear() {
    for (const auto &p : m_registry) {
        delete (Creator<> *) p.second;
    }

    m_registry = {};
}
}

#include <algine/core/TypeRegistry.h>
#include <algine/core/Engine.h>

#include <stdexcept>

using namespace std;

namespace algine {
STATIC_INITIALIZER_IMPL(TypeRegistry) {
    Engine::addOnDestroyListener([]() {
        TypeRegistry::clear();
    });
}

void TypeRegistry::registerType(const string &name, void *creator) {
    registry()[name] = creator;
}

void TypeRegistry::unregister(const string &name) {
    if (auto it = registry().find(name); it != registry().end()) {
        delete (Creator<> *) it->second;
        registry().erase(it);
    } else {
        throw runtime_error("Type '" + name + "' does not found");
    }
}

bool TypeRegistry::contains(const string &name) {
    return registry().find(name) != registry().end();
}

TypeRegistry::Deleter TypeRegistry::getDeleter(const string &name) {
    return ((Creator<> *) registry()[name])->getDeleter();
}

void TypeRegistry::clear() {
    for (const auto &p : registry()) {
        delete (Creator<> *) p.second;
    }

    registry() = {};
}

std::map<std::string, void *> &TypeRegistry::registry() {
    static std::map<std::string, void*> registry;
    return registry;
}
}

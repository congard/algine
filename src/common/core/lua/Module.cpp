#include <algine/core/lua/Module.h>

namespace algine {
Module::Module()
    : m_loaders() {}

Module::Module(std::initializer_list<Loader> loaders)
    : Module()
{
    for (auto &loader : loaders) {
        addLoader(loader);
    }
}

Module::Module(const Loader &loader)
    : Module()
{
    addLoader(loader);
}

Module::Module(const Module &other) {
    *this = other;
}

Module& Module::operator=(const Module &other) {
    if (this == &other)
        return *this;

    m_loaders = other.m_loaders;

    return *this;
}

Module::Module(Module &&other) noexcept {
    *this = std::move(other);
}

Module& Module::operator=(Module &&other) noexcept {
    if (this == &other)
        return *this;

    std::swap(m_loaders, other.m_loaders);

    return *this;
}

void Module::addLoader(const Loader &loader) {
    m_loaders.emplace_front(loader);
}

void Module::load(const Args &args, sol::environment &env) {
    for (auto &loader : m_loaders) {
        loader(args, env);
    }
}
} // algine
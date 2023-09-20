#ifndef ALGINE_MODULELOADER_H
#define ALGINE_MODULELOADER_H

#include <algine/platform.h>

#include <functional>
#include <forward_list>
#include <string>

#include <sol/environment.hpp>

namespace algine {
class AL_EXPORT Module {
public:
    using Args = std::forward_list<std::string>;

    /**
     * @param args module arguments, i.e. module-name:arg1:arg2:...:argn
     * @param env environment
     */
    using Loader = std::function<void(const Args &args, sol::environment &env)>;

public:
    Module();
    Module(std::initializer_list<Loader> loaders);
    explicit Module(const Loader &loader);

    Module(const Module &other);
    Module& operator=(const Module &other);

    Module(Module &&other) noexcept;
    Module& operator=(Module &&other) noexcept;

    void addLoader(const Loader &loader);

    void load(const Args &args, sol::environment &env);

private:
    using LoaderList = std::forward_list<Loader>;
    LoaderList m_loaders;
};
} // algine

#endif //ALGINE_MODULELOADER_H

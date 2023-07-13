#ifndef ALGINE_LUA_H
#define ALGINE_LUA_H

#include <algine/core/io/IOProvider.h>
#include <sol/state.hpp>

#include <memory>
#include <mutex>
#include <map>

namespace algine_lua {
template<typename T> void registerLuaUsertype(sol::table &table, void *userdata);
}

namespace algine {
class Lua: public IOProvider {
public:
    template<typename... Types>
    struct TypeList {};

    class Locker {
    public:
        explicit Locker(const std::unique_ptr<std::recursive_mutex> &mutex);
        explicit Locker(const Lua *lua);
        ~Locker();

        Locker(const Locker&) = delete;
        Locker(Locker&&) = delete;
        Locker& operator=(const Locker&) = delete;
        Locker& operator=(Locker&&) = delete;

    private:
        std::recursive_mutex *m_mutex;
    };

    using ModuleArgs = std::forward_list<std::string>;

    /**
     * @param args module arguments, i.e. module-name:arg1:arg2:...:argn
     * @param env environment
     */
    using ModuleLoader = std::function<void(const ModuleArgs &args, const sol::environment &env)>;

public:
    void init();
    bool isInitialized() const;

    void setThreadSafety(bool enabled);
    bool isThreadSafety() const;
    const std::unique_ptr<std::recursive_mutex>& getMutex() const;

    const std::unique_ptr<sol::state>& state() const;

    sol::global_table createEnvironment();
    sol::global_table createEnvironment(const sol::global_table &parent);
    sol::global_table& getGlobalEnvironment() const;

    /**
     * Adds module loader
     * <br>Module can be loaded from Lua with
     * <br><code>require("algine:module-name")</code>
     * @param name module-name
     * @param loader
     */
    void addModuleLoader(std::string_view name, const ModuleLoader &loader);

    /**
     * Removes module loader
     * @param name module-name
     */
    void removeModuleLoader(std::string_view name);

    static sol::global_table& getEnv(Lua *lua, sol::global_table *env = nullptr);
    static bool isRegistered(sol::global_table &env, std::string_view type);

    template<typename T, typename G, typename S>
    static void new_property(sol::usertype<T> &usertype,
        std::string_view propName, std::string_view getterName, std::string_view setterName,
        G &&getter, S &&setter);

    template<typename T, typename G, typename S>
    static void new_property(sol::usertype<T> &usertype, std::string_view propName, G &&getter, S &&setter);

    template<typename T, typename... Args>
    void registerUsertype(sol::global_table *env = nullptr);

    template<typename... Args>
    void registerUsertype(sol::global_table *env, TypeList<Args...>) { registerUsertype<Args...>(env); }

    template<typename T>
    static void registerUsertype(Lua *lua = nullptr, sol::global_table *tenv = nullptr) {
        auto &env = Lua::getEnv(lua, tenv);
        sol::table table = env;
        algine_lua::registerLuaUsertype<T>(table, lua);
    }

    static Lua& getDefault();

private:
    void initEnvironment(sol::global_table &env);
    void loadModule(const ModuleArgs &args, const sol::environment &env);

private:
    std::unique_ptr<sol::state> m_lua {nullptr};
    std::unique_ptr<std::recursive_mutex> m_mutex {nullptr};
    std::map<std::string, ModuleLoader> m_moduleLoaders;

private:
    static Lua m_default;
};

template<typename T, typename G, typename S>
void Lua::new_property(sol::usertype<T> &usertype,
    std::string_view propName, std::string_view getterName, std::string_view setterName,
    G &&getter, S &&setter)
{
    usertype[propName] = sol::property(getter, setter);
    usertype[getterName] = getter;
    usertype[setterName] = setter;
}

template<typename T, typename G, typename S>
void Lua::new_property(sol::usertype<T> &usertype, std::string_view propName, G &&getter, S &&setter) {
    std::string prop = propName.data();
    prop[0] = static_cast<char>(toupper(prop[0]));
    new_property(usertype, propName, "get" + prop, "set" + prop, getter, setter);
}

template<typename T, typename... Args>
void Lua::registerUsertype(sol::global_table *env) {
    registerUsertype<T>(this, env);

    if constexpr(sizeof...(Args) != 0) {
        registerUsertype<Args...>(env);
    }
}
} // algine

#endif //ALGINE_LUA_H

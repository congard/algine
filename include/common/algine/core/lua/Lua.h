#ifndef ALGINE_LUA_H
#define ALGINE_LUA_H

#include <algine/core/io/IOProvider.h>
#include <algine/core/lua/Module.h>

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

    class Locker;

    using TypeLoader = std::function<void(sol::environment &env)>;

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
     * Adds module
     * <br>Module can be loaded from Lua with
     * <br><code>require("algine:module-name")</code>
     * @param name module-name
     * @param module
     * @throws std::runtime_error if a module with
     * the same name already exists
     */
    void addModule(std::string_view name, const Module &module);

    /**
     * Returns a module by the provided name
     * @note if such a module doesn't exist, it will be created
     * @param name
     * @return
     */
    Module& getModule(std::string_view name);

    /**
     * @param name Module name
     * @return true, if such a module exists, false otherwise
     */
    bool hasModule(std::string_view name);

    /**
     * Removes module
     * @param name module-name
     */
    void removeModule(std::string_view name);

    /**
     * Adds a custom type loader.
     * @param name The name of the type.
     * @param loader The loader.
     * @note This function can be safely called during
     * static initialization.
     */
    static void addTypeLoader(std::string_view name, TypeLoader loader);

    /**
     * Checks whether a type loader with the specified
     * name exists.
     * @param name The type name.
     * @return `true` if it exists, `false` otherwise.
     * @note This function can be safely called during
     * static initialization.
     */
    static bool hasTypeLoader(std::string_view name);

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

    /**
     * @return The default instance.
     * @note This function can be safely called during
     * static initialization.
     */
    static Lua& getDefault();

private:
    void initEnvironment(sol::global_table &env);
    void loadModule(const Module::Args &args, sol::environment &env);

private:
    std::unique_ptr<sol::state> m_lua {nullptr};
    std::unique_ptr<std::recursive_mutex> m_mutex {nullptr};
    std::map<std::string, Module> m_modules;

private:
    static Lua& get_default();
    static std::map<std::string, TypeLoader>& get_typeLoaders();
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

class Lua::Locker {
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
} // algine

#endif //ALGINE_LUA_H

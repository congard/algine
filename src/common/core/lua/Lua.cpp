#include <algine/core/lua/Lua.h>
#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/core/log/Log.h>
#include <algine/platform.h>

#include <algine/core/math/Rect.h>
#include <algine/core/math/Point.h>

#include <tulz/demangler.h>

#include <utility>

#include <sol/variadic_args.hpp>

#include "AlgineBindings.h"
#include "CoreLua.h"

namespace algine {
Lua::Locker::Locker(std::recursive_mutex *mutex)
    : m_mutex(mutex)
{
    if (m_mutex) {
        m_mutex->lock();
    }
}

Lua::Locker::Locker(const Lua *lua)
    : Locker(lua ? lua->getMutex() : Lua::getDefault().getMutex()) {}

Lua::Locker::~Locker() {
    if (m_mutex) {
        m_mutex->unlock();
    }
}

void Lua::init() {
    Locker locker(this);
    m_lua = std::make_unique<sol::state>();
    m_lua->open_libraries();

    // TODO: move to static initializer lists
    addModule("glm", Module([this](auto &, sol::environment &env) { GLMLuaTypes::registerLuaUsertype(this, &env); }));
    addModule("tulz", Module([this](auto &, sol::environment &env) { TulzLuaTypes::registerLuaUsertype(this, &env); }));

    sol::environment env = getGlobalEnvironment();
    initEnvironment(env);
}

bool Lua::isInitialized() const {
    Locker locker(this);
    return m_lua != nullptr;
}

void Lua::setThreadSafety(bool enabled) {
    if (enabled && !m_mutex) {
        m_mutex = std::make_unique<std::recursive_mutex>();
    } else if (!enabled) {
        m_mutex.reset();
    }
}

bool Lua::isThreadSafety() const {
    return m_mutex.get();
}

std::recursive_mutex* Lua::getMutex() const {
    return m_mutex.get();
}

sol::state& Lua::state() {
    Locker locker(this);
    if (m_lua == nullptr)
        init();
    return *m_lua;
}

sol::environment Lua::createEnvironment() {
    Locker locker(this);
    sol::environment env(state(), sol::create);
    initEnvironment(env);
    return env;
}

sol::global_table& Lua::getGlobalEnvironment() {
    return state().globals();
}

void Lua::addModule(std::string_view name, const Module &module) {
    m_modules[name.data()] = module;
}

Module& Lua::getModule(std::string_view name) {
    if (auto it = m_modules.find(name.data()); it != m_modules.end()) {
        return it->second;
    } else {
        return (m_modules[name.data()] = Module());
    }
}

bool Lua::hasModule(std::string_view name) {
    return m_modules.find(name.data()) != m_modules.end();
}

void Lua::removeModule(std::string_view name) {
    m_modules.erase(name.data());
}

void Lua::loadModule(const Module::Args &args, sol::environment &env) {
    auto &name = args.front();

    if (auto it = m_modules.find(name); it != m_modules.end()) {
        it->second.load(args, env);
    } else {
        throw std::runtime_error("Module " + name + " cannot be loaded: loader not found");
    }
}

template<typename T, typename... Args>
bool static registerType(std::string_view type, sol::environment &table, Lua *lua) {
    if (tulz::demangler::demangle(typeid(T).name()) == type) {
        algine_lua::registerLuaUsertype<T>(table, lua);
        return true;
    }

    if constexpr (sizeof...(Args) > 0)
        return registerType<Args...>(type, table, lua);

    return false;
}

template<typename... Args>
bool static registerType(std::string_view type, sol::environment &table, Lua *lua, Lua::TypeList<Args...>) {
    return registerType<Args...>(type, table, lua);
}

Lua& Lua::getDefault() {
    auto &def = get_default();
    return def;
}

void Lua::initEnvironment(sol::environment &env) {
    auto luaUsertype = env.new_usertype<Lua>(
            "Lua",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<IOProvider>());
    luaUsertype["memoryUsed"] = [this]() { return m_lua->memory_used(); };

    auto print = [](bool err, const sol::variadic_args &va, sol::this_environment tenv) {
        sol::function toString = (*tenv.env)["tostring"];
        std::string message;

        for (auto it = va.begin() + 1; it < va.end(); ++it) {
            message += toString(*it).get<std::string_view>();
            message += '\t';
        }

        if (!message.empty()) {
            message.erase(message.size() - 1);

            if (!err) {
                Log::info("AlgineLua", message);
            } else {
                Log::error("AlgineLua", message);
            }
        }
    };

    luaUsertype["print"] = [print](const sol::variadic_args &va, sol::this_environment tenv) { print(false, va, std::move(tenv)); };
    luaUsertype["printErr"] = [print](const sol::variadic_args &va, sol::this_environment tenv) { print(true, va, std::move(tenv)); };

    env["state"] = this;

    env["isLinux"] = sol::readonly_property(sol::var(is_linux()));
    env["isAndroid"] = sol::readonly_property(sol::var(is_android()));
    env["isWindows"] = sol::readonly_property(sol::var(is_windows()));

    env["algine_require_module"] = [this](std::string_view moduleInfo, sol::this_environment tenv) {
        Module::Args args;
        auto last = args.before_begin();

        size_t startPos = 0;
        size_t pos;

        while ((pos = moduleInfo.find(':', startPos + 1)) != std::string_view::npos) {
            last = args.insert_after(last, std::string(moduleInfo.data() + startPos, pos - startPos));
            startPos = pos + 1;
        }

        // insert last
        args.insert_after(last, std::string(moduleInfo.data() + startPos));

        loadModule(args, *tenv.env);
    };

    env["algine_require_type"] = [this](std::string_view type, sol::this_environment tenv) {
        sol::environment &env = tenv;

        auto typeReg = [&]<typename T>() {
            return [&]() { algine_lua::registerLuaUsertype<T>(env, this); };
        };

        // TODO: move to static initializers
        std::map<std::string_view, std::function<void()>> customReg = {
            {"algine::PointI", typeReg.operator()<PointI>()},
            {"algine::PointF", typeReg.operator()<PointF>()},
            {"algine::RectI", typeReg.operator()<RectI>()},
            {"algine::RectF", typeReg.operator()<RectF>()}
        };

        if (auto it = customReg.find(type); it != customReg.end()) {
            it->second();
        } else if (!registerType(type, env, this, AlgineBindings())) {
            get_typeLoaders()[type.data()](*tenv.env);
        }
    };

    registerUsertype<IOProvider>(this, &env);

    m_lua->script(core_lua, env);
}

sol::environment Lua::getEnv(Lua *lua, sol::environment *env) {
    if (env)
        return *env;

    if (!lua)
        return getDefault().state().globals();

    if (!lua->isInitialized())
        lua->init();

    return lua->state().globals();
}

bool Lua::isRegistered(sol::environment &env, std::string_view type) {
    return env[type].valid();
}

void Lua::addTypeLoader(std::string_view name, TypeLoader loader) {
    get_typeLoaders()[name.data()] = std::move(loader);
}

bool Lua::hasTypeLoader(std::string_view name) {
    auto &loaders = get_typeLoaders();
    return loaders.find(name.data()) != loaders.end();
}

Lua& Lua::get_default() {
    static Lua defaultLua;
    return defaultLua;
}

std::map<std::string, Lua::TypeLoader>& Lua::get_typeLoaders() {
    static std::map<std::string, TypeLoader> typeLoaders;
    return typeLoaders;
}
} // algine
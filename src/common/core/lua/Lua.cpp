#include <algine/core/lua/Lua.h>
#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/core/log/Log.h>
#include <algine/templates.h>

#include <algine/core/math/Rect.h>
#include <algine/core/math/Point.h>

#include <utility>
#include <cxxabi.h>

#include <sol/variadic_args.hpp>

#include "AlgineBindings.h"
#include "CoreLua.h"

namespace algine {
Lua::Locker::Locker(const std::unique_ptr<std::recursive_mutex> &mutex)
    : m_mutex(mutex.get())
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
    addModule("glm", Module([this](auto &, sol::global_table env) { GLMLuaTypes::registerLuaUsertype(this, &env); }));
    addModule("tulz", Module([this](auto &, sol::global_table env) { TulzLuaTypes::registerLuaUsertype(this, &env); }));

    initEnvironment(getGlobalEnvironment());
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

const std::unique_ptr<std::recursive_mutex>& Lua::getMutex() const {
    return m_mutex;
}

const std::unique_ptr<sol::state>& Lua::state() const {
    return m_lua;
}

sol::global_table Lua::createEnvironment() {
    Locker locker(this);
    sol::environment env(*m_lua, sol::create);
    sol::global_table tenv = env;
    initEnvironment(tenv);
    return tenv;
}

sol::global_table Lua::createEnvironment(const sol::global_table &parent) {
    Locker locker(this);
    return sol::environment(*m_lua, sol::create, sol::environment(parent));
}

sol::global_table& Lua::getGlobalEnvironment() const {
    return m_lua->globals();
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

std::string nameof(const char* tn) {
    const auto dmg = abi::__cxa_demangle(tn, nullptr, nullptr, nullptr);
    std::string name = dmg;
    std::free(dmg);
    return name;
}

template<typename T, typename... Args>
bool registerType(std::string_view type, sol::table &table, Lua *lua) {
    if (nameof(typeid(T).name()) == type) {
        algine_lua::registerLuaUsertype<T>(table, lua);
        return true;
    }

    if constexpr (sizeof...(Args) > 0)
        return registerType<Args...>(type, table, lua);

    return false;
}

template<typename... Args>
bool registerType(std::string_view type, sol::table &table, Lua *lua, Lua::TypeList<Args...>) {
    return registerType<Args...>(type, table, lua);
}

Lua& Lua::getDefault() {
    auto &def = get_default();
    if (!def.isInitialized())
        def.init();
    return def;
}

template<typename T>
struct type_holder {
    using type = T;
};

void Lua::initEnvironment(sol::global_table &env) {
    auto usertype = env.new_usertype<Lua>(
            "Lua",
            sol::meta_function::construct, sol::no_constructor,
            sol::call_constructor, sol::no_constructor,
            sol::base_classes, sol::bases<IOProvider>());
    usertype["memoryUsed"] = [this]() { return m_lua->memory_used(); };

    auto print = [](bool err, const sol::variadic_args &va, sol::this_environment tenv) {
        sol::function toString = sol::environment(*tenv.env)["tostring"];
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

    usertype["print"] = [print](const sol::variadic_args &va, sol::this_environment tenv) { print(false, va, std::move(tenv)); };
    usertype["printErr"] = [print](const sol::variadic_args &va, sol::this_environment tenv) { print(true, va, std::move(tenv)); };

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
        sol::global_table env = *tenv.env;
        sol::table table = env;

        auto typeReg = [&](auto typeHolder) {
            using T = typename decltype(typeHolder)::type;
            return [&]() { algine_lua::registerLuaUsertype<T>(table, this); };
        };

        // TODO: move to static initializers
        std::map<std::string_view, std::function<void()>> customReg = {
            {"algine::PointI", typeReg(type_holder<PointI>())},
            {"algine::PointF", typeReg(type_holder<PointF>())},
            {"algine::RectI", typeReg(type_holder<RectI>())},
            {"algine::RectF", typeReg(type_holder<RectF>())}
        };

        if (auto it = customReg.find(type); it != customReg.end()) {
            it->second();
        } else if (!registerType(type, table, this, AlgineBindings())) {
            get_typeLoaders()[type.data()](*tenv.env);
        }
    };

    registerUsertype<IOProvider>(this, &env);

    m_lua->script(core_lua, sol::environment(env));
}

sol::global_table& Lua::getEnv(Lua *lua, sol::global_table *env) {
    if (env)
        return *env;

    if (!lua)
        return getDefault().state()->globals();

    if (!lua->isInitialized())
        lua->init();

    return lua->state()->globals();
}

bool Lua::isRegistered(sol::global_table &env, std::string_view type) {
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
#include <algine/core/lua/Lua.h>
#include <algine/core/lua/GLMLuaTypes.h>
#include <algine/core/lua/TulzLuaTypes.h>
#include <algine/templates.h>

#include <utility>
#include <sol/variadic_args.hpp>

#include "AlgineCore.h"
#include "AlgineStd.h"
#include "CoreLua.h"

namespace algine {
Lua::Locker::Locker(const std::unique_ptr<std::recursive_mutex> &mutex): m_mutex(mutex.get()) {
    if (m_mutex) {
        m_mutex->lock();
    }
}

Lua::Locker::Locker(const Lua *lua): Locker(lua ? lua->getMutex() : Engine::getLua().getMutex()) {}

Lua::Locker::~Locker() {
    if (m_mutex) {
        m_mutex->unlock();
    }
}

void Lua::init() {
    Locker locker(this);
    m_lua = std::make_unique<sol::state>();
    m_lua->open_libraries();
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

    env["algine_require"] = [this](std::string_view lib, sol::this_environment tenv) {
        sol::global_table env = *tenv.env;

        if (lib == "core") {
            registerUsertype(&env, AlgineCore());
        } if (lib == "std") {
            registerUsertype(&env, AlgineStd());
        } else if (lib == "glm") {
            GLMLuaTypes::registerLuaUsertype(this, &env);
        } else if (lib == "tulz") {
            TulzLuaTypes::registerLuaUsertype(this, &env);
        }
    };

    registerUsertype<IOProvider>(this, &env);

    m_lua->script(core_lua, sol::environment(env));
}

sol::global_table& Lua::getEnv(Lua *lua, sol::global_table *env) {
    if (env)
        return *env;

    if (!lua)
        return Engine::getLua().state()->globals();

    if (!lua->isInitialized())
        lua->init();

    return lua->state()->globals();
}

bool Lua::isRegistered(sol::global_table &env, std::string_view type) {
    return env[type].valid();
}
} // algine